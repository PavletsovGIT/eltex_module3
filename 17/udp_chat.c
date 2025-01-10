#include <stdlib.h>
#include "srvclilib.h"

int main(int argc, char* argv[]) {
    int pid, port, rv, host_port;
    struct hostent* host;

    if (argc != 4) {
        printf("Programm needs 3 argument :\n");
        printf("\tprogramm's server-port\n");
        printf("\tfrind's IP addr\n");
        printf("\tfriend's server-port\n");
        exit(EXIT_FAILURE);
    }

    port = atoi(argv[1]);
    /*
    if ((host = gethostname(argv[2])) == NULL) {
        perror("gethostbyname");
        exit(EXIT_FAILURE);
    }
    */
    host_port = atoi(argv[3]);

    switch (pid = fork()) {
        case -1 :
            perror("fork");
            exit(EXIT_FAILURE);
        case 0: // Child Proccess
            udpclient(argv[2], host_port);
            break;
        default: // Parent Process
            udpserver(port);
            wait(&rv);
            break;
    }

    return 0;
}
