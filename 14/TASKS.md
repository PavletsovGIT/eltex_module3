# Tasks

## Issues

1.Исправить стрелочки ввода/вывода соообщений
На данный момент:

``` bash
$ ./chat.out 57123 127.0.0.1 51123 
> Hello
> <Hi
```

Должно быть:

``` bash
$ ./chat.out 57123 127.0.0.1 51123 
> Hello
>
< Hi
>
```

2.При отправке сообщения `exit`, программа Отправитель не завершается. Возможно только завершает дочерний процесс.

Возможные причины:

1. Клиентский процесс Отправителя обработал слово `exit`, так как он его и отправил, а серверный процесс Отправителя не получил сообщения для окончания процесса.

Идеи для исправления:

1. Добавить логгер и посмотреть, что и как заершается.
2. Проверить логику завершения программы.
