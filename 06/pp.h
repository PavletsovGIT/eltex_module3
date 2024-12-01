#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>

#define QUEUE_KEY 1234
#define MSG_SIZE 100
#define CLI_PRIORITY 1  // Сообщение от клиента к серверу
#define SRV_PRIORITY 2  // Сообщение от сервера к клиенту
#define END_PRIORITY 3  // Сообщение для завершения

struct msgbuf {
    long mtype;
    char mtext[MSG_SIZE];
};

void start_server();
void start_client();
void cleanup_queue();
