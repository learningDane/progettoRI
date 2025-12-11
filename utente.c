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
        printf("porta da collegare: %d\n", atoi(argv[1]));
    }

    // addr mio
    struct sockaddr_in my_addr, lavagna;
    memset(&my_addr, 0, (size_t)sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = port;
    inet_pton(AF_INET, LOCALHOST, &my_addr.sin_addr);

    // addr lavagna
    //int lavagnaLen = sizeof(lavagna);
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

    //HELLO(&mysock,&lavagna);

    char buf[MAX_LEN_COMANDO + 2]; // buffer per il comando da terminale, dimensionato per il comando più lungo possibile + \n + \0

    while(1) {
        printf("Inserire un comando:\n");
        fgets(buf, sizeof(buf), stdin); // questo legge al più (numero di caratteri disponibili in buf - 1), ed in fondo aggiunge \0
        // rimozione di \n:
        for (int i = 0; buf[i] != '\0'; i++) {
            if (buf[i] == '\n') {
                buf[i] = '\0';
            }
        }
        int ID_comando = VALIDA_INPUT(buf);
        if (ID_comando < 0) {
            printf("comando non riconosciuto. Riprovare...\n");
        }
        else if (ID_comando > 4) { // per ogni comando chiamare la funzione corretta
            printf("Comando non disponibile agli utenti. Riprovare...\n");
        }
        else if (ID_comando == 0) {
            CREATE_CARD();
        }
        else if (ID_comando == 1) {
            HELLO(&mysock, (struct sockaddr_in *)&lavagna);
        }
        else if (ID_comando == 2) {
            QUIT();
        }
        else if (ID_comando == 3) {
            ACK_CARD();
        }
        else if (ID_comando == 4) {
            CARD_DONE();
        }
    }


    // prima azione è registrazione

    // quando ricevo AVAILABLE_CARD comincia uno scambio di messaggi per assegnare la carta

    // 1. HELLO


    // N. QUIT
    close(mysock);
    return 0;
}
