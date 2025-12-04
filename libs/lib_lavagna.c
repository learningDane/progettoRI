#include "../include/include_lavagna.h"

int utentiConnessi = 0;
int mysock; // mysock è il socket di ascolto
uint16_t ID_LAVAGNA = 6969;
socklen_t utenteLen;
fd_set readfds; // set di descrittori da controllare per la lettura
card_t *testaToDo = NULL;
card_t *testaDoing = NULL;
card_t *testaDone = NULL;
struct utente_des * testa_des_utente = NULL;
struct sockaddr utente;


struct utente_des {
    utente_t utente;
    int des;
    struct utente_des*prox;
};


void SHOW_LAVAGNA() {
    // stampa la lavagna, con le colonne e le card assegnate nelle giuste colonne
    printf("----------------------------\n|          %5d           |\n----------------------------\n| ToDo  |  Doing  |  Done  |\n----------------------------\n",ID_LAVAGNA);
    card_t*todo = testaToDo;
    card_t*doing = testaDoing;
    card_t*done = testaDone;
    if (todo == NULL && doing == NULL && done == NULL) { return; }
    while (todo != NULL && doing != NULL && done != NULL) {
        if (todo != NULL) {
            printf("| %5d |", todo->ID);
        }
        else {
            printf("|       |");
        }

        if (doing != NULL) {
            printf(" %5d |", doing->ID);
        }
        else {
            printf("       |");
        }

        if (done != NULL) {
            printf(" %5d |\n", done->ID);
        }
        else {
            printf("       |");
        }
    }
    printf("----------------------------\n");
}


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
