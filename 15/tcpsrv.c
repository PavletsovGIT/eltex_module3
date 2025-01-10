/* Пример простого TCP сервера
   Порт является аргументом, для запуска сервера неободимо ввести:
   # ./[имя_скомпилированного_файла] [номер порта]
   # ./server 57123
*/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

#define BUFFSIZE 1024
#define OPSCOUNT 4 // Количество доступных функций

typedef struct operation_t {
	int(*action)(int, int); // указатель на функцию
	char name[20]; // 20 символов на наазвание функции
} operation_t;

// прототип функции, обслуживающей
// подключившихся пользователей
void dostuff(int);

// Функция обработки ошибок
void error(const char *msg)
{
    perror(msg);
    exit(1);
}

// глобальная переменная – количество
// активных пользователей 
int nclients = 0;

// макрос для печати количества активных
// пользователей 
void printusers()
{ 
	if(nclients)
	{printf("%d user on-line\n",nclients);}
	else {printf("No User on line\n");}
}

// функция по варианту
int myfunc(int a, int b)
{
	return a+b;
}

int diff(int a, int b)
{
	return a - b;
}

int mult(int a, int b)
{
	return a - b;
}

int division(int a, int b)
{
	return (b == 0) ? 0 : (a / b);
}

const operation_t operations[OPSCOUNT] = {
	{
		.name = "summ",
		.action = myfunc
	},
	{
		.name = "diff",
		.action = diff
	},
	{
		.name = "mult",
		.action = mult
	},
	{
		.name = "div",
		.action = division
	}
};

int main(int argc, char *argv[])
{
    char buff[BUFFSIZE];    // Буфер для различных нужд
	printf("TCP SERVER DEMO\n");
	
	int sockfd, newsockfd; // дескрипторы сокетов
	int portno; // номер порта
	int pid; // id номер потока
    socklen_t client; // размер адреса клиента типа socklen_t
    struct sockaddr_in serv_addr, cli_addr; // структура сокета сервера и клиента
	
	// ошибка в случае если мы не указали порт
    if (argc < 2) 
	{
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }
    // Шаг 1 - создание сокета
	// AF_INET     - сокет IPv4
    // SOCK_STREAM  - потоковый сокет (с
    //      установкой соединения)
    // 0 - по умолчанию выбирается TCP протокол
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
    // ошибка при создании сокета
	if (sockfd < 0) {
       error("ERROR opening socket");
	}

	// Шаг 2 - связывание сокета с локальным адресом
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY; // сервер принимает подключения на все IP-адреса
    serv_addr.sin_port = htons(portno);
    // вызываем bind для связывания
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		error("ERROR on binding");
	}
    // Шаг 3 - ожидание подключений, размер очереди - 5
	listen(sockfd,5);
    client = sizeof(cli_addr);
	
	// Шаг 4 - извлекаем сообщение из очереди
	// цикл извлечения запросов на подключение из очереди    
	while (1) 
	{
        newsockfd = accept(sockfd,(struct sockaddr *) &cli_addr, &client); // принимаем клиентский запрос на соединение
        if (newsockfd <= 0) {
			error("ERROR on accept");
		}
		nclients++; // увеличиваем счетчик
    	// подключившихся клиентов
		/*
		// вывод сведений о клиенте
		struct hostent *hst;
		hst = gethostbyaddr((char *)&cli_addr.sin_addr, 4, AF_INET);
		printf("+%s [%s] new connect!\n",
		(hst) ? hst->h_name : "Unknown host",
		(char*)inet_ntoa(cli_addr.sin_addr));*/
		printusers();

		switch (pid = fork()) {
			case -1:
				error("fork");
			case 0:
				close(sockfd);
            	dostuff(newsockfd);
            	exit(0);
            default:
            	close(newsockfd);

		}
    } /* end of while */
    close(sockfd);
    return 0; /* we never get here */
}

void dostuff (int sock)
{
	//int n,a,b;
	int bytes_recv; // размер принятого сообщения
	int a,b; // переменные для myfunc
	char buff[BUFFSIZE];
	#define str0 "Enter func name\n"
	#define str1 "Enter 1 parametr\n"
	#define str2 "Enter 2 parametr\n"
	char action_name[20];
	int(*action)(int, int) = NULL;

    write(sock, str0, sizeof(str0));
	bytes_recv = read(sock, buff, sizeof(buff));
	strncpy(action_name, buff, strlen(buff)); // можно добавить проверку на длинну строки
	for (int i = 0; i < OPSCOUNT; i++) { // Выбираем функцию по имени
		if (strcmp(action_name, operations[i].name) == 0) {
			action = operations[i].action;
			break;
		}
	}

	if (action == NULL) {
		write(sock, "quit", 4);
		nclients--; // уменьшаем счетчик активных клиентов
    	printf("-disconnect\n"); 
		printusers();
		return;
	}
	
	// отправляем клиенту сообщение
   	write(sock, str1, strlen(str1));
   
   	// обработка первого параметра
	bytes_recv = read(sock,&buff[0],sizeof(buff));
	if (bytes_recv < 0) error("ERROR reading from socket");
	a = atoi(buff); // преобразование первого параметра в int
	
	// отправляем клиенту сообщение
	write(sock,str2,strlen(str2));
	
	bytes_recv = read(sock,&buff[0],sizeof(buff));
	if (bytes_recv < 0) error("ERROR reading from socket");
	
	b = atoi(buff); // преобразование второго параметра в int
	a = action(a,b); // вызов пользовательской функции
	snprintf(buff, sizeof(buff), "%d", a); // преобразование результата выполнения в строку
	buff[strlen(buff)] = '\n'; // добавление к сообщению символа конца строки
	
	// отправляем клиенту результат
	write(sock,&buff[0],sizeof(buff));
	
	nclients--; // уменьшаем счетчик активных клиентов
    	printf("-disconnect\n"); 
	printusers();
	return;
}