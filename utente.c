#include "libs/lib_utente.c"

int main (int argc, char *argv[]) {
    if (argc != 2) {
        printf("ERR:\tincludere come argomento il numero di porta\n");
        return -1;
    }
    else if (atoi(argv[1]) < 5679) {
        printf("inserire un numero di porta maggiore di 5678\n");
        return -1;
    }
    // argv[0] : comando eseguito
    // argv[1] : numero di porta, (parte da 5679 incluso)

    // connessione TCP bloccante alla lavagna
    int port = htons(atoi(argv[1]));
    if (DEBUG) {
        printf("porta da collegare: %d\n", port);
    }

    // addr mio
    struct sockaddr_in my_addr, lavagna;
    memset(&my_addr, 0, (size_t)sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = port;
    inet_pton(AF_INET, LOCALHOST, &my_addr.sin_addr);

    // addr lavagna
    int lavagnaLen = sizeof(lavagna);
    memset(&lavagna, 0, (size_t)sizeof(lavagna));
    lavagna.sin_family = AF_INET;
    lavagna.sin_port = PORTA_LAVAGNA;
    inet_pton(AF_INET, LOCALHOST, &lavagna.sin_addr);

    int mysock = socket(AF_INET, SOCK_STREAM, 0);
    if (DEBUG) {
        printf("Aperto socket %d\n", mysock);
    }

    int err_bind = bind(mysock, (struct sockaddr*)&my_addr, (socklen_t)sizeof(my_addr));
    if (err_bind) {
        perror("errore su bind: ");
        return -1;
    }
    if (DEBUG) {
        printf("Socket collegato\n");
    }

    HELLO(&mysock,&lavagna);


    // prima azione è registrazione

    // quando ricevo AVAILABLE_CARD comincia uno scambio di messaggi per assegnare la carta

    // 1. HELLO


    // N. QUIT
    close(mysock);
    return 0;
}
