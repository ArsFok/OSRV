
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "protocol.h"

#define SOCKET_PATH "/tmp/otp_server.sock"

int main(void) {
    int client_fd = socket(AF_UNIX, SOCK_STREAM, 0);

    if(client_fd == -1){
        perror("socket");
        return EXIT_FAILURE;
    }

    struct sockaddr_un address = {0};

    address.sun_family = AF_UNIX;

    snprintf(address.sun_path, sizeof(address.sun_path), "%s", SOCKET_PATH);


    if(connect(client_fd, (struct sockaddr *)&address, sizeof(address)) == -1){
        perror("connect");
        close(client_fd);
        return EXIT_FAILURE;
    }
  
    // Создаём запрос
    struct request req = {0};

    printf("Введите приоритет (0-10): ");
    if(scanf("%d", &req.priority) != 1){
         fprintf(stderr, "Некорректный приоритет\n");
        close(client_fd);
        return EXIT_FAILURE;
    }

    printf("Введите команду (1 - ENCRYPT, 2 - TIMER, 3 - STATUS): ");
    if (scanf("%d", &req.command) != 1) {
        fprintf(stderr, "Некорректная команда\n");
        close(client_fd);
        return EXIT_FAILURE;
    }

    printf("Введите значение: ");
    if (scanf("%d", &req.value) != 1) {
        fprintf(stderr, "Некорректное значение\n");
        close(client_fd);
        return EXIT_FAILURE;
    }

    if (req.priority < 0 || req.priority > 10) {
        fprintf(stderr, "Приоритет должен быть от 0 до 10\n");
        close(client_fd);
        return EXIT_FAILURE;
    }

    // Отправляем запрос серверу
    ssize_t bytes = write(client_fd, &req, sizeof(req));

    if(bytes == -1){
        perror("write");
        close(client_fd);
        return EXIT_FAILURE;
    }

    if (bytes != sizeof(req)) {
        fprintf(stderr, "Запрос отправлен не полностью\n");
        close(client_fd);
        return EXIT_FAILURE;
    }

    // Создаём структуру для получения ответа
    struct response resp = {0};

    // Читаем ответ сервера
    ssize_t bytes_read = read(client_fd, &resp, sizeof(resp));

    if(bytes_read == -1){
        perror("read");
        close(client_fd);
        return EXIT_FAILURE;
    }

    if(bytes_read != sizeof(resp)){
        fprintf(stderr, "Ответ получен не полностью\n");
        close(client_fd);
        return EXIT_FAILURE;
    }

    // Выводим ответ
    printf("[CLIENT] Статус: %d\n", resp.status);
    printf("[CLIENT] Результат: %d\n", resp.result);

    // Закрываем соединение
    close(client_fd);

    return EXIT_SUCCESS;
}
