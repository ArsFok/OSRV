#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "protocol.h"
#include "io.h"

#define SOCKET_PATH "/tmp/otp_server.sock"

struct response process_request(struct request req){
    struct response resp = {0};

    switch (req.command)
    {
    case CMD_ENCRYPT:
        resp.result = req.value ^ 42;
        resp.status = 0;
        break;
    case CMD_STATUS:
        resp.result = 1;
        resp.status = 0;
        break;
    case CMD_TIMER:
        resp.status = -1;
        break;
    default:
        resp.status = -1;
        break;
    }
    return resp;
}

int main(){
    // создаем серверный сокет:
    int server_fd = socket(AF_UNIX, SOCK_STREAM, 0);

    if(server_fd == -1){
        perror("socket");
        return EXIT_FAILURE;
    }
    // Указываем адрес сокета

    struct sockaddr_un address = {0};

    address.sun_family = AF_UNIX;

    snprintf(address.sun_path, sizeof(address.sun_path), "%s", SOCKET_PATH);

    // Удаляем файл сокета от предыдущего запроса
    unlink(SOCKET_PATH);

    // Привязываем сокет к адресу
    if(bind(server_fd, (struct sockaddr *)&address, sizeof(address)) == -1){
        perror("bind");
        close(server_fd);
        return EXIT_FAILURE;
    }
    if(listen(server_fd, 5) == -1){
        perror("listen");
        close(server_fd);
        unlink(SOCKET_PATH);
        return EXIT_FAILURE;
    }

    printf("[SERVER] Ожидание клиента...\n");

    // Принимаем подключение
    while(1){
        int client_fd = accept(server_fd, NULL, NULL);

        if(client_fd == -1){
            perror("accept");
            break;
        }

        printf("[SERVER] Клиент подключился\n");

        // Получаем запрос 

        struct request req =  {0};

        ssize_t bytes = read_full(client_fd, &req, sizeof(req));
        
        if(bytes != sizeof(req)){
            fprintf(stderr, "Ошибка чтения запроса\n");
            close(client_fd);
            continue;
        }

        // Вводим информацию
        printf("[SERVER] Получено число:\n");
        printf("Приоритет: %d\n", req.priority);
        printf("Команда: %d\n", req.command);
        printf("Значение: %d\n", req.value);

        // Обрабатываем запрос
        struct response resp = process_request(req);

        // Отправляем ответ
        ssize_t bytes_written = write_full(client_fd, &resp, sizeof(resp));

        if(bytes_written == -1){
            perror("write");
        }else if(bytes_written != sizeof(resp)){
            fprintf(stderr, "Ответ отправлен не полностью\n");
        }else{
            printf("[SERVER] Ответ отправлен:\n");
            printf("Статус: %d\n", resp.status);
            printf("Результат: %d\n", resp.result);
        }

        // закрываем клиента
        close(client_fd);
    }
    // Освободаем ресурсы сервера
    close(server_fd);
    unlink(SOCKET_PATH);
    return EXIT_SUCCESS;
}