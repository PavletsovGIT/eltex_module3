#include "pp.h"

const char SRV_QUEUE_NAME[] = "/srv-chat";
const char CLI_QUEUE_NAME[] = "/cli-chat";

void start_server() {
    mqd_t qd_server, qd_client;
    char in_buffer[MSG_BUFFER_SIZE], out_buffer[MAX_MSG_SIZE];
    struct mq_attr attr;

    attr.mq_flags = 0;
    attr.mq_maxmsg = MAX_MESSAGES;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;

    // Открытие серверной очереди
    if ((qd_server = mq_open(SRV_QUEUE_NAME, O_RDWR | O_CREAT, QUEUE_PERMISSIONS, &attr)) == -1) {
        perror("Server: mq_open");
        exit(EXIT_FAILURE);
    }

    // Получение имени клиентской очереди
    if (mq_receive(qd_server, in_buffer, MSG_BUFFER_SIZE, NULL) == -1) {
        perror("Server: mq_receive");
        exit(1);
    }

    printf("Received client queue name: %s\n", in_buffer);

    // Открываем очередь клиента
    if ((qd_client = mq_open(in_buffer, O_WRONLY)) == -1) {
        perror("Server: Not able to open client queue");
        exit(EXIT_FAILURE);
    }

    while (1) {
        // Получение сообщения от клиента
        if (mq_receive(qd_server, in_buffer, MSG_BUFFER_SIZE, NULL) == -1) {
            perror("Server: mq_receive");
            exit(1);
        }

        printf("Received message: %s\n", in_buffer);

        if (strcmp(in_buffer, "exit") == 0) {
            printf("Closing chat...\n");
            break;
        }

        // Чтение ответа для клиента
        printf("Server# ");
        fgets(out_buffer, MAX_MSG_SIZE, stdin);
        out_buffer[strcspn(out_buffer, "\n")] = '\0';

        // Отправка ответа клиенту
        if (mq_send(qd_client, out_buffer, strlen(out_buffer) + 1, 0) == -1) {
            perror("Server: Not able to send message to client");
        }

        if (strcmp(out_buffer, "exit") == 0) {
            printf("Closing chat...\n");
            break;
        }
    }

    // Закрытие очередей
    mq_close(qd_client);
    mq_close(qd_server);
    mq_unlink(SRV_QUEUE_NAME);
}


void start_client() {
    mqd_t qd_client, qd_server;
    char in_buffer[MAX_MSG_SIZE], out_buffer[MAX_MSG_SIZE];
    struct mq_attr attr;

    attr.mq_flags = 0;
    attr.mq_maxmsg = MAX_MESSAGES;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;

    if ((qd_client = mq_open(CLI_QUEUE_NAME, O_RDONLY | O_CREAT, 
                             QUEUE_PERMISSIONS, &attr)) == -1) {
        perror("Client: mq_open (client)");
        exit(EXIT_FAILURE);
    }

    if ((qd_server = mq_open(SRV_QUEUE_NAME, O_WRONLY)) == -1) {
        perror("client: mq_open (server)");
        exit(1);
    }

    // Отправляем имя клиентской очереди серверу
    if (mq_send(qd_server, CLI_QUEUE_NAME, strlen(CLI_QUEUE_NAME) + 1, 0) == -1) {
        perror("Client: Unable to send client queue name");
        exit(EXIT_FAILURE);
    }

    while (1) {
        printf("Client# ");
        fgets(out_buffer, MAX_MSG_SIZE, stdin);
        out_buffer[strcspn(out_buffer, "\n")] = '\0';

        if (mq_send(qd_server, out_buffer, strlen(out_buffer) + 1, 0) == -1) {
            perror("Client: Unable to send message to server");
        }

        if (strcmp(out_buffer, "exit") == 0) {
            printf("Closing chat...\n");
            break;
        }

        if (mq_receive(qd_client, in_buffer, MSG_BUFFER_SIZE, NULL) == -1) {
            perror("Client: mq_receive");
            exit(1);
        }

        printf("Received from server: %s\n", in_buffer);

        if (strcmp(in_buffer, "exit") == 0) {
            printf("Closing chat...\n");
            break;
        }
    }

    mq_close(qd_client);
    mq_unlink(CLI_QUEUE_NAME);
}
