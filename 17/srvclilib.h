#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>

#define BUFFER_SIZE 1024

void udpserver(int port);
void udpclient(char* srv_ip, int port);