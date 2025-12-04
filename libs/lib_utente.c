#include "../include/include_utente.h"

int rando() {
    srand(time(NULL));
    int n = rand();
    return n;
}


void HELLO(int*mysock, struct sockaddr_in*lavagna) {
    // lavagna risponde con card in ToDo
    int err_connect = connect(*mysock, (const struct sockaddr*)lavagna, (socklen_t)sizeof(*lavagna));
    if (err_connect) {
        perror("Errore connect: ");
        exit(-1);
    }
    if (DEBUG) {
        printf("Connesso alla lavagna\n");
    }
}
