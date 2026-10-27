
#include "io.h"

#include <stdio.h>
#include <unistd.h>
#include <errno.h>

#define IO_DEBUG 1

ssize_t read_full(int fd, void *buffer, size_t count) {
    size_t total = 0;
    char *ptr = buffer;

    if (IO_DEBUG) {
        fprintf(stderr, "[IO][READ] Начало чтения: %zu байт\n",
                count);
    }

    while (total < count) {
        ssize_t bytes = read(fd, ptr + total, count - total);

        if (bytes == -1) {
            if (errno == EINTR) {
                if (IO_DEBUG) {
                    fprintf(stderr,
                            "[IO][READ] Прервано сигналом, повтор\n");
                }
                continue;
            }

            if (IO_DEBUG) {
                perror("[IO][READ] Ошибка");
            }
            return -1;
        }

        if (bytes == 0) {
            if (IO_DEBUG) {
                fprintf(stderr, "[IO][READ] Соединение закрыто\n");
            }
            break;
        }

        total += (size_t)bytes;

        if (IO_DEBUG) {
            fprintf(stderr,
                    "[IO][READ] Получено: %zd, всего: %zu/%zu байт\n",
                    bytes, total, count);
        }
    }

    return (ssize_t)total;
}

ssize_t write_full(int fd, const void *buffer, size_t count) {
    size_t total = 0;
    const char *ptr = buffer;

    if (IO_DEBUG) {
        fprintf(stderr, "[IO][WRITE] Начало отправки: %zu байт\n",
                count);
    }

    while (total < count) {
        ssize_t bytes = write(fd, ptr + total, count - total);

        if (bytes == -1) {
            if (errno == EINTR) {
                if (IO_DEBUG) {
                    fprintf(stderr,
                            "[IO][WRITE] Прервано сигналом, повтор\n");
                }
                continue;
            }

            if (IO_DEBUG) {
                perror("[IO][WRITE] Ошибка");
            }
            return -1;
        }

        if (bytes == 0) {
            if (IO_DEBUG) {
                fprintf(stderr,
                        "[IO][WRITE] Нет прогресса при отправке\n");
            }
            break;
        }

        total += (size_t)bytes;

        if (IO_DEBUG) {
            fprintf(stderr,
                    "[IO][WRITE] Отправлено: %zd, всего: %zu/%zu байт\n",
                    bytes, total, count);
        }
    }

    return (ssize_t)total;
}
