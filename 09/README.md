# Lab 9

## Task

Скорректировать программу 08 так, чтобы несколько процессов (ограниченное количество) могли читать из файла. Запись в файл возможна, когда он никем не читается

Задание 08 (Семафоры System V, 2 балла)
Скорректировать программу 05 так, чтобы доступ к файлу регулировался семафором.

Задание 05 (Сигналы, 3 балла).
Изменить программу 04 так, чтобы дочерний процесс выводил информацию из файла.
Если родительский процесс собирается изменить файл, то он отправляет сигнал SIGUSR1 (блокировка доступа к файлу).
Когда родительский процесс завершил модификацию файла, он отправляет сигнал SIGUSR2 (разрешение доступа к файлу).
Дочерний процесс отправляет новое число родительскому после того, как прочитал файл.

Задание 04 (Каналы, 2 балла).
Написать программу, порождающую дочерний процесс и использующую однонаправленный обмен данными.
Процесс-потомок генерирует случайные числа и отправляет родителю.
Родитель выводит числа на экран и в файл.
Количество чисел задается в параметрах запуска приложения.

## BUGS

1. Sometimes child processes can end life-cicle too early and give error `semop: Invalid argument`.
For example:
``` bash
$ ./a.out 6 7
Parent PID 5256,	wrote:	67
Child PID 5257,	read:	67
Child PID 5258,	read:	67
Child PID 5259,	read:	67
Child PID 5260,	read:	67
Child PID 5263,	read:	67
Child PID 5262,	read:	67
Child PID 5261,	read:	67
Parent PID 5256,	wrote:	78
Child PID 5257,	read:	78
Child PID 5261,	read:	78
Child PID 5260,	read:	78
Child PID 5259,	read:	78
Child PID 5258,	read:	78
Child PID 5262,	read:	78
Child PID 5263,	read:	78
Parent PID 5256,	wrote:	27
Child PID 5257,	read:	27
Child PID 5263,	read:	27
Child PID 5258,	read:	27
Child PID 5259,	read:	27
Child PID 5260,	read:	27
Child PID 5261,	read:	27
Child PID 5262,	read:	27
Parent PID 5256,	wrote:	10
Child PID 5257,	read:	10
Child PID 5258,	read:	10
Child PID 5259,	read:	10
Child PID 5261,	read:	10
Child PID 5262,	read:	10
Child PID 5260,	read:	10
Child PID 5263,	read:	10
Parent PID 5256,	wrote:	13
Child PID 5259,	read:	13
Child PID 5261,	read:	13
Child PID 5262,	read:	13
Child PID 5257,	read:	13
Child PID 5260,	read:	13
Child PID 5258,	read:	13
Child PID 5263,	read:	13
Parent PID 5256,	wrote:	26
Child PID 5258,	read:	26
Child PID 5257,	read:	26
Child PID 5263,	read:	26
Child process exited with status 0
Child PID 5259,	read:	26
semop: Invalid argument
Child PID 5261,	read:	26
semop: Invalid argument
Child PID 5262,	read:	26
semop: Invalid argument
Child PID 5260,	read:	26
semop: Invalid argument

$ ./a.out 3 7
Parent PID 5302,	wrote:	78
Child PID 5303,	read:	78
Child PID 5305,	read:	78
Child PID 5306,	read:	78
Child PID 5307,	read:	78
Child PID 5308,	read:	78
Child PID 5304,	read:	78
Child PID 5309,	read:	78
Parent PID 5302,	wrote:	7
Child PID 5303,	read:	78
Child PID 5306,	read:	78
Child PID 5307,	read:	78
Child PID 5308,	read:	78
Child PID 5305,	read:	78
Child PID 5304,	read:	78
Child PID 5309,	read:	78
Parent PID 5302,	wrote:	38
Child PID 5307,	read:	38
Child PID 5308,	read:	38
Child PID 5306,	read:	38
Child PID 5305,	read:	38
Child PID 5303,	read:	38
Child PID 5304,	read:	38
Child PID 5309,	read:	38
Child process exited with status 0

$ ./a.out 4 7
Parent PID 5346,	wrote:	25
Child PID 5347,	read:	25
Child PID 5348,	read:	25
Child PID 5349,	read:	25
Child PID 5350,	read:	25
Child PID 5351,	read:	25
Child PID 5352,	read:	25
Child PID 5353,	read:	25
Parent PID 5346,	wrote:	34
Child PID 5351,	read:	34
Child PID 5350,	read:	34
Child PID 5349,	read:	34
Child PID 5348,	read:	34
Child PID 5352,	read:	34
Child PID 5347,	read:	34
Child PID 5353,	read:	34
Parent PID 5346,	wrote:	99
Child PID 5352,	read:	99
Child PID 5348,	read:	99
Child PID 5349,	read:	99
Child PID 5350,	read:	99
Child PID 5347,	read:	99
Child PID 5351,	read:	99
Child PID 5353,	read:	99
Parent PID 5346,	wrote:	58
Child PID 5347,	read:	58
Child PID 5351,	read:	58
Child PID 5349,	read:	58
Child PID 5350,	read:	58
Child PID 5348,	read:	58
Child PID 5352,	read:	58
Child PID 5353,	read:	58
Child process exited with status 0
```