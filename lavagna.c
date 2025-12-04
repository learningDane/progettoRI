#include "include/include.h"
#include <stdio.h>
#include <string.h>

int utentiConnessi;
card_t *testaToDo;
card_t *testaDoing;
card_t *testaDone;

int main () {
    // 1. mostra a video lo stato della lavagna
    // SHOW_LAVAGNA();

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

    int mysock = socket(AF_INET, SOCK_STREAM, 0);
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

    int n; // = numero di descrittori da controllare
    fd_set *readfds;
    FD_ZER0(&readfds);
    select(n, readfds, NULL, NULL, NULL);
    /*
    select(int nfds, fd_set *restrict readfds, fd_set *restrict writefds, fd_set *restrict errorfds, struct timeval *restrict timeout);
    */

    int utente_sock = accept(mysock, (struct sockaddr *)&utente, (socklen_t *)&utenteLen);
    if (errno) {
        perror("errore su accept: ");
        return -1;
    }
    printf("richiesta di connessione accettata\n");
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
