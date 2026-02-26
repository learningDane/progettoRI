#include "include/include_utente.h"

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

    mysock_utente = socket(AF_INET, SOCK_STREAM, 0);
    if (DEBUG) {
        printf("Aperto socket %d\n", mysock_utente);
    }

    int err_bind = bind(mysock_utente, (struct sockaddr*)&my_addr, (socklen_t)sizeof(my_addr));
    if (err_bind) {
        perror("errore su bind: ");
        return -1;
    }
    if (DEBUG) {
        printf("Socket collegato\n");
    }

    //HELLO(&mysock_utente,&lavagna);

    char buf[MAX_LEN_COMANDO + 2]; // buffer per il comando da terminale, dimensionato per il comando più lungo possibile + \n + \0

    while(1) {
        printf("Inserire un comando:\n");

        int des = rileva_input();
        switch (des) {
            case 0: {
                // non ci sono messaggi, caso impossibile
                /// <---
                break;
            }
            case 3: {
                // messaggi sia da lavagna che da terminale
                DBG(MESSAGGIO SIA DA LAVAGNA CHE DA TERMINALE);
                /// <---
                break;
            }
            case 1: {
                // messaggio dalla lavagna
                /// <---
                // leggi mess
                // if (!read()) {QUIT();}
                gestisci_messaggio_lavagna();
                break;
            }
            case 2: {
                // messaggio dal terminale
                DBG(Gestione comando da terminale.);
                fgets(buf, sizeof(buf), stdin); // questo legge al più (numero di caratteri disponibili in buf - 1), ed in fondo aggiunge \0
                // rimozione di \n:
                for (int i = 0; buf[i] != '\0'; i++) {
                    if (buf[i] == '\n') {
                        buf[i] = '\0';
                    }
                }
                int ID_comando = VALIDA_INPUT(buf);

                switch (ID_comando) {
                    case -1: {
                        printf("comando non riconosciuto. Riprovare...\n");
                        break;
                    }
                    case ID_HELLO: {
                        HELLO(&mysock_utente, (struct sockaddr_in *)&lavagna);
                        break;
                    }
                    case ID_QUIT: {
                        QUIT();
                    }
                    case ID_CREATE_CARD: {
                        CREATE_CARD();
                        break;
                    }
                    case ID_CARD_DONE: {
                        CARD_DONE();
                        break;
                    }
                    default: {
                        printf("Comando non disponibile.\n");
                    }
                }
            }
        }
    }
    close(mysock_utente);
    DBG(chiusura.);
    return 0;
}
