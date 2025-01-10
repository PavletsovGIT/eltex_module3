#include "pp.h"

void cleanup_queue() {
    int msgid;
    // Удаляем очередь сообщений
    if ((msgid = msgget(QUEUE_KEY, 0666)) != -1) {
        msgctl(msgid, IPC_RMID, NULL);
        printf("Очередь сообщений удалена.\n");
    }
}

void start_server() {
    int msgid;
    int n;
    struct msgbuf message;

    // Создаем/подключаемся к очереди сообщений
    if ((msgid = msgget(QUEUE_KEY, IPC_CREAT | 0666)) == -1) {
        perror("msgget");
        exit(1);
    }

    while (1) {
        // Ожидаем сообщение от клиента
        if (msgrcv(msgid, &message, MSG_SIZE, CLI_PRIORITY, 0) == -1) {
            perror("msgrcv");
            exit(1);
        }
        printf("From Client: %s\n", message.mtext);

        // Проверяем завершение
        if (strcmp(message.mtext, "exit") == 0) {
            break;
        }

        // Отправляем ответ клиенту
        printf("Srv: ");
        fgets(message.mtext, MSG_SIZE, stdin);
        message.mtext[strcspn(message.mtext, "\n")] = '\0';  // Убираем символ новой строки

        if (strcmp(message.mtext, "exit") == 0) {
            message.mtype = END_PRIORITY;
            msgsnd(msgid, &message, strlen(message.mtext) + 1, 0);
            break;
        } else {
            message.mtype = SRV_PRIORITY;
            msgsnd(msgid, &message, strlen(message.mtext) + 1, 0);
        }
    }

    cleanup_queue();
}

void start_client() {
    int msgid;
    struct msgbuf message;

    // Подключаемся к очереди сообщений
    if ((msgid = msgget(QUEUE_KEY, IPC_CREAT | 0666)) == -1) {
        perror("msgget");
        exit(1);
    }

    while (1) {
        // Отправляем сообщение серверу
        printf("Cli: ");
        fgets(message.mtext, MSG_SIZE, stdin);
        message.mtext[strcspn(message.mtext, "\n")] = '\0';  // Убираем символ новой строки

        if (strcmp(message.mtext, "exit") == 0) {
            message.mtype = END_PRIORITY;
            msgsnd(msgid, &message, strlen(message.mtext) + 1, 0);
            break;
        } else {
            message.mtype = CLI_PRIORITY;
            msgsnd(msgid, &message, strlen(message.mtext) + 1, 0);
        }

        // Ожидаем ответ от сервера
        if (msgrcv(msgid, &message, MSG_SIZE, SRV_PRIORITY, 0) == -1) {
            perror("msgrcv");
            exit(1);
        }
        printf("From Server: %s\n", message.mtext);

        if (strcmp(message.mtext, "exit") == 0) {
            break;
        }
    }

    cleanup_queue();
}