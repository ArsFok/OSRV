#ifndef PROTOCOL_H
#define PROTOCOL_H

// Команды устройства

#define CMD_ENCRYPT 1
#define CMD_TIMER   2
#define CMD_STATUS  3

// Структура запроса

struct request {
    int priority;
    int command;
    int value;
};

// Структура ответа

struct response {
    int status;
    int result;
};


#endif