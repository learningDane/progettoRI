#include "../include/include_lavagna.h"

int utentiConnessi = 0;
int mysock; // mysock è il socket di ascolto
int max_des;
char comando[256]; // buffer per il comando da terminale
uint16_t ID_LAVAGNA = 6969;
fd_set readfds; // set di descrittori da controllare per la lettura
card_t *testaToDo = NULL;
card_t *testaDoing = NULL;
card_t *testaDone = NULL;
char buf[MAX_LEN_COMANDO + 2]; // buffer per il comando da terminale, dimensionato per il comando più lungo possibile + \n + \0

struct utente_des * testa_des_utente = NULL;
struct sockaddr_in utente;
socklen_t utenteLen = sizeof(utente);

// struttura contentente porta utente, descrittore del relativo socket e puntatore per realizzare la lista
struct utente_des {
    // numero porta utente
    utente_t utente;
    // descrittore socket relativo all'utente
    int des;
    //puntatore al successivo utente
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
    if (des_attuale == NULL) {
        // gestisci errore
    }
    while (des_attuale->prox != NULL) {
        des_attuale = des_attuale->prox;
    }
    return des_attuale;
}


void accetta_utente() {
    if (DEBUG) {
        printf("Richiesta di connessione sotto esame.\n");
    }
    int utente_sock = accept(mysock, (struct sockaddr *)&utente, (socklen_t *)&utenteLen);
    if (errno) {
        perror("errore su accept: ");
        exit(-1);
    }
    if (DEBUG) {
        printf("richiesta di connessione accettata\n");
        utentiConnessi++;
    }

    // aggiungi sock alla lista di des_utenti
    struct utente_des*nuovo_utente = malloc(sizeof(struct utente_des));
    if (nuovo_utente == NULL) {
        printf("malloc error\n");
        exit(-1);
    }
    nuovo_utente->utente = ntohs(utente.sin_port); // salvo in utente_des il numero di porta dell'utente
    nuovo_utente->des = utente_sock; // salvo in utente_des il descrittore del socket relativo all'utente
    nuovo_utente->prox = NULL; // è l'ultimo elemento nella lista

    if (DEBUG) {
        printf("accettazione utente %d\n", nuovo_utente->utente);
    }

    if (testa_des_utente == NULL) {
        testa_des_utente = nuovo_utente;
    }
    else {
        struct utente_des*ultimo_utente = scorri_lista(testa_des_utente);
        ultimo_utente->prox = nuovo_utente;
    }

    // rispondo con ACK all'utente
    send(nuovo_utente->des, "OK", 2, 0);
    if (DEBUG) {
        printf("messaggio inviato: %s\n", "OK\0");
    }

    STAMPA_UTENTI();

    return;
}


void prepara_set() {
    max_des = 0; // Azzera il massimo all'inizio di ogni ciclo

    // 1. Aggiungo il descrittore del socket di ascolto
    FD_SET(mysock, &readfds);
    if (mysock > max_des) {
        max_des = mysock;
    }

    // 2. Aggiungo il descrittore di stdin
    FD_SET(STDIN_FILENO, &readfds);
    if (STDIN_FILENO > max_des) { // Di solito STDIN_FILENO è 0, ma è buona norma
        max_des = STDIN_FILENO;
    }

    // 3. Aggiungo i descrittori dei socket connessi con gli utenti
    struct utente_des *des_attuale = testa_des_utente;
    while (des_attuale != NULL) {
        FD_SET(des_attuale->des, &readfds);
        if (des_attuale->des > max_des) {
            max_des = des_attuale->des;
        }
        des_attuale = des_attuale->prox;
    }
}

void STAMPA_UTENTI() {
    printf("====== Utenti Connessi ======\n");
    struct utente_des*utente_attuale = testa_des_utente;
    if (utente_attuale == NULL) {
        printf("nessun utente connesso.\n");
        printf("=============================\n");
    }
    printf("numero di utenti connessi: %d\n",utentiConnessi);
    while (utente_attuale != NULL) {
        printf("\t%d\n",utente_attuale->utente);
        utente_attuale = utente_attuale->prox;
    }
    printf("=============================\n");
}

void input_stdin() {
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
}
