#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

#define MAXSTRSIZE 1000
#define PORT 6789

int main()
{
    int sockfd; /* Дескриптор сокета */
    int clilen, n; /* Переменные для различных длин 
        и количества символов */
    char line[MAXSTRSIZE]; /* Массив для принятой и 
        отсылаемой строки */
    struct sockaddr_in servaddr, cliaddr; /* Структуры 
        для адресов сервера и клиента */
    /* Заполняем структуру для адреса сервера: семейство
    протоколов TCP/IP, сетевой интерфейс – любой, номер порта 
    51000. Поскольку в структуре содержится дополнительное не
    нужное нам поле, которое должно быть нулевым, перед 
    заполнением обнуляем ее всю */
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    /* Создаем UDP сокет */
    if((sockfd = socket(PF_INET, SOCK_DGRAM, 0)) < 0){
        perror(NULL); /* Печатаем сообщение об ошибке */
        exit(1);
    }
    /* Настраиваем адрес сокета */
    if(bind(sockfd, (struct sockaddr *) &servaddr, 
    sizeof(servaddr)) < 0){
        perror(NULL);
        close(sockfd);
        exit(1);
    }
    while(1) {
        /* Основной цикл обслуживания*/
        /* В переменную clilen заносим максимальную длину
        для ожидаемого адреса клиента */
        clilen = sizeof(cliaddr);
        /* Ожидаем прихода запроса от клиента и читаем его. 
        Максимальная допустимая длина датаграммы – MAXSTRSIZE - 1 
        символов, адрес отправителя помещаем в структуру 
        cliaddr, его реальная длина будет занесена в 
        переменную clilen */
        if((n = recvfrom(sockfd, line, MAXSTRSIZE - 1, 0, 
        (struct sockaddr *) &cliaddr, &clilen)) < 0){
            perror(NULL);
            close(sockfd);
            exit(1);
        }
        /* Печатаем принятый текст на экране */
        printf("From client: %s\n", line);
        
        printf("To client: ");
        /* Вводим текст для ответа клиенту*/
        fgets(line, MAXSTRSIZE, stdin);
        line[strcspn(line, "\n") - 1] = '\0';

        if(sendto(sockfd, line, strlen(line), 0, 
        (struct sockaddr *) &cliaddr, clilen) < 0){
            perror(NULL);
            close(sockfd);
            exit(1);
        } /* Уходим ожидать новую датаграмму*/
    }
    return 0;
}