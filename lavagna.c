#include "libs/lib_lavagna.c"

int main () {
    // 1. mostra a video lo stato della lavagna
    SHOW_LAVAGNA();

    // ad ogni spostamento o creazione di una card, mostra la lavagna

    // se una card è in Doing da troppo tempo (1minuto.30s), ping utente, se non riceve risposta dopo 30 secondi suppone che si sia disconnesso

    // se ci sono almeno due utenti viene mandato a tutti AVAILABLE_CARD.
    // Questa operazione verrà ripetuta ogni volta che un utente ha terminato una card e ci sono almeno due utenti presenti e card disponibili.


    //////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////inizializzazione socket di ascolto nuove connessioni
    int port = PORTA_LAVAGNA;
    printf("porta da collegare: %d\n", port);

    // accettazione connessioni TCP dagli utenti tramite socket non bloccanti e I/O multiplexing
    // connessione TCP bloccante

    struct sockaddr_in my_addr, utente;
    int utenteLen = sizeof(utente);
    memset(&my_addr, 0, (size_t)sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = port;
    inet_pton(AF_INET, LOCALHOST, &my_addr.sin_addr);

    mysock = socket(AF_INET, SOCK_STREAM, 0);
    printf("Aperto socket %d\n", mysock);

    int err_bind = bind(mysock, (struct sockaddr*)&my_addr, (socklen_t)sizeof(my_addr));
    if (err_bind) {
        perror("errore su bind: ");
        return -1;
    }
    printf("Socket collegato\n");

    int err_listen = listen(mysock, BACKLOG);
    if (err_listen) {
        perror("errore su listen: ");
        return -1;
    }
    printf("socket in ascolto per nuove connessioni...\n");
    ////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////

    int n = 0; // = numero di descrittori da controllare
    FD_ZERO(&readfds);
    select(n, &readfds, NULL, NULL, /*timeout*/NULL);
    /*
    select(int nfds, fd_set *restrict readfds, fd_set *restrict writefds, fd_set *restrict errorfds, struct timeval *restrict timeout);
    */

    accetta_utente();
}

/*
    la lavagna è identificata da:
        - ID
        - colonne
        - card
    le card sono almeno 10
    ogni card è identificata da:
        - ID
        - colonna
        - testo attività
        - utente che la sta facendo oppure utente che l'ha fatta
        - timestamp dell'ultima modifica
    gli utenti sono identificati dal loro numero di porta (incrementale a partire dalla 5679)
    per prima cosa l'utente contatta la lavagna e si registra
*/
