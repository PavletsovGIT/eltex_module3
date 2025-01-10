# Task 5

## Main information
Задание 05 (Сигналы, 3 балла).
Изменить программу 04 так, чтобы дочерний процесс выводил информацию из файла.
Если родительский процесс собирается изменить файл, то он отправляет сигнал SIGUSR1 (блокировка доступа к файлу).
Когда родительский процесс завершил модификацию файла, он отправляет сигнал SIGUSR2 (разрешение доступа к файлу).
Дочерний процесс отправляет новое число родительскому после того, как прочитал файл.

## Bugs

1. Программа корректно работает только во время отладки.
1.2. Без отладки программа либо не отрабоатывает дочерний процесс и зависает, либо отрабатывает, но по вывод такой:

``` bash
$ ./a.out 2
PARENT: CHILD PID = 12764
PARENT PID = 12763
CHILD PID = 12764
PID: 12764 : message = data0
PID: 12763 : RAND NUM = 71
```

А дальше программа зависает.

## Ideas

1. Нужно синхронизировать работу процессов