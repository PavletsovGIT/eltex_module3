#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 12345
#define BUFFER_SIZE 1024

int main() {
    int sockfd;
    struct sockaddr_in server_addr, client1_addr, client2_addr;
    char buffer[BUFFER_SIZE];
    socklen_t addr_len = sizeof(struct sockaddr_in);

    // Создаем UDP-сокет
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Настраиваем адрес сервера
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Привязываем сокет к адресу
    if (bind(sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Сервер запущен и ожидает сообщения...\n");

    int client1_ready = 0, client2_ready = 0;

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        struct sockaddr_in client_addr;
        int n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &addr_len);
        if (n < 0) {
            perror("recvfrom failed");
            continue;
        }

        if (!client1_ready) {
            client1_addr = client_addr;
            client1_ready = 1;
            printf("Первый клиент подключился: %s:%d\n",
                   inet_ntoa(client1_addr.sin_addr), ntohs(client1_addr.sin_port));
        } else if (!client2_ready && 
                   (client_addr.sin_port != client1_addr.sin_port ||
                    client_addr.sin_addr.s_addr != client1_addr.sin_addr.s_addr)) {
            client2_addr = client_addr;
            client2_ready = 1;
            printf("Второй клиент подключился: %s:%d\n",
                   inet_ntoa(client2_addr.sin_addr), ntohs(client2_addr.sin_port));
        }

        printf("Получено сообщение: %s\n", buffer);

        // Пересылка сообщения другому клиенту
        if (client1_ready && client2_ready) {
            struct sockaddr_in *target_addr = 
                (client_addr.sin_port == client1_addr.sin_port && 
                 client_addr.sin_addr.s_addr == client1_addr.sin_addr.s_addr) 
                ? &client2_addr : &client1_addr;

            sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)target_addr, addr_len);
        }
    }

    close(sockfd);
    return 0;
}
