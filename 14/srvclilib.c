#include "srvclilib.h"

void udpserver(int port) {
    int sockfd, bytes_rcv;
    struct sockaddr_in srv_addr, cli_addr;
    socklen_t addr_len = sizeof(struct sockaddr_in);
    char buff[BUFFER_SIZE];

    // Создаём udp-сокет
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket");
        return;
    }
    
    // Настраиваем адрес сервера
    memset(&srv_addr, 0, addr_len); // Выставляем всё в 0
    srv_addr.sin_family = AF_INET; // IPv4
    srv_addr.sin_addr.s_addr = INADDR_ANY; // Принимает от всех адресов
    srv_addr.sin_port = htons(port); // Устанавливаем рабочий порт

    // Делаем связку дескриптора сокета и  сетевых данных сервера
    if (bind(sockfd, (const struct sockaddr *)&srv_addr, sizeof(srv_addr))) {
        perror("bind");
        close(sockfd);
        return;
    }

    while (1) {
        memset(buff, 0, BUFFER_SIZE); // Отчищаем буфер от мусора
        memset(&cli_addr, 0, addr_len); // Отчищаем данные предыдущего клиента
        /* Получам данные
         * recvfrom - читает пакет из sockfd, пишет данные в buffer,
         * максимальный размер получаемых данных - BUFFER_SIZE,
         * записывает данные отправителя в cli_addr
        */
        if ((bytes_rcv = recvfrom(sockfd, buff, BUFFER_SIZE, 0, 
                          (struct sockaddr *)&cli_addr, &addr_len)) < 0) {
            perror("recvfrom failed");
            continue;
        }
        
        printf("<%s\n", buff);

        // Если получено exit, то сервер прекращает работу.
        if (strncmp(buff, "exit", 4) == 0) break;
    }

    close(sockfd);
    exit(EXIT_SUCCESS);
}

void udpclient(char* srv_ip, int port) {
    int sockfd, bytes_rcv = 0;
    struct sockaddr_in srv_addr;
    char buff[BUFFER_SIZE];

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket");
        return;
    }

    memset(&srv_addr, 0, sizeof(srv_addr));
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, srv_ip, &srv_addr.sin_addr) <= 0) {
        perror("inet_pton, Invalid address");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    while (1) {
        memset(buff, 0, BUFFER_SIZE);
        printf("> ");
        fgets(buff, BUFFER_SIZE, stdin);

        sendto(sockfd, buff, strlen(buff), 0, (struct sockaddr *)&srv_addr, sizeof(srv_addr));

        if (strncmp(buff, "exit", 4) == 0) {
            break;
        }
    }

    close(sockfd);
    memset(&srv_addr, 0, sizeof(srv_addr));
    exit(EXIT_SUCCESS);
}