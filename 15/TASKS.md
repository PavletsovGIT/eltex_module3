# Tasks

## Баги

1. Клиент при запуске не получает сообщений от сервера. Может быть клиени не подключается к серверу?

### Баг №1

#### Проблема

При запуске сначала сервера, а потом клиента, сервер не обрабатывает запрос на подключение от клиента (строка: ```newsockfd = accept(sockfd,(struct sockaddr *) &cli_addr, &client); // принимаем клиентский запрос на соединение```). Клиент после запуска останавливается на строке ```if (connect(my_sock,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {``` и примерно 4 секунды ожидает подключения, после чего выводит ошибку `ERROR connecting: Connection refused`.

#### Как пробовал исправить

1. Команда ```netstat -tnul``` - выводит список занятых портов. При просмотре списка занятых портов напротив порта приложения сервера в статусе было написано `LISTEN`.

``` bash
tcp        0      0 0.0.0.0:57123           0.0.0.0:*               LISTEN  
```

2. Было предположение что сервер не правильно привязывается к сокету, потому что сокет в прошлые разы работы программы оставался открытым после завершения работы процесса. Но эта версия не подходит, потому что при смене сокета проблема не исчезает.