#include "include/include.h"
#include <stdio.h>
#include <string.h>

int utentiConnessi = 0;
card_t *testaToDo = NULL;
card_t *testaDoing = NULL;
card_t *testaDone = NULL;
// mysock è il socket di ascolto
int mysock;
struct sockaddr utente;
socklen_t utenteLen;

struct utente_des {
    utente_t utente;
    int des;
    struct utente_des*prox;
};

struct utente_des * testa_des_utente = NULL;

/* @brief Restituisce il puntatore all'ultimo descrittore di socket per utente
 *
 * @param puntatore alla lista di descrittori da scorrere
 */
struct utente_des* scorri_lista (struct utente_des* des_attuale) {
    while (des_attuale->prox != NULL) {
        des_attuale = des_attuale->prox;
    }
    return des_attuale->prox;
}

void accetta_utente() {
    int utente_sock = accept(mysock, (struct sockaddr *)&utente, (socklen_t *)&utenteLen);
    if (errno) {
        perror("errore su accept: ");
        exit(-1);
    }
    printf("richiesta di connessione accettata\n");

    // aggiungi sock alla lista di des_utenti
    struct utente_des nuovo_utente;
    nuovo_utente.utente = 0;
    nuovo_utente.des = utente_sock;
    nuovo_utente.prox = NULL;

    struct utente_des*ultimo_utente = scorri_lista(testa_des_utente);
    ultimo_utente->prox = &nuovo_utente;
    return;
}

// set di descrittori da controllare per la lettura
fd_set readfds;

/* @brief Aggiunge al set di descrittori readfds ogni socket da controllare con select()
 */
void prepara_set() {
    // aggiungo il descrittore del socket di ascolto
    FD_SET(mysock, &readfds);

    // aggiungo i descrittori dei socket connessi con gli utenti
    struct utente_des*des_attuale = testa_des_utente;
    do {
        /* Aggiungere un descrittore al set */
        FD_SET(des_attuale->des, &readfds);
        des_attuale = des_attuale->prox;
    }
    while (des_attuale->prox != NULL);
}

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
