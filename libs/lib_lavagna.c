#include "../include/include_lavagna.h"

int utentiConnessi = 0;
int mysock_lavagna; // mysock_lavagna è il socket di ascolto
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


void SHOW_LAVAGNA() {
    // stampa la lavagna, con le colonne e le card assegnate nelle giuste colonne
    printf("-------------------------------------------\n|                   %5d                 |\n-------------------------------------------\n|    ToDo    |     Doing    |     Done    |\n-------------------------------------------\n",ID_LAVAGNA);
    card_t*todo = testaToDo;
    card_t*doing = testaDoing;
    card_t*done = testaDone;
    if (todo == NULL && doing == NULL && done == NULL) { return; }
    while (!(todo == NULL && doing == NULL && done == NULL)) {
        if (todo != NULL) {
            printf("| %10d |", todo->ID);
            todo = todo->prox;
        }
        else {
            printf("|            |");
        }

        if (doing != NULL) {
            printf(" %10d |", doing->ID);
            doing = doing->prox;
        }
        else {
            printf("              |");
        }

        if (done != NULL) {
            printf(" %10d |\n", done->ID);
            done = done->prox;
        }
        else {
            printf("             |\n");
        }
    }
    printf("-------------------------------------------\n");
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
    int utente_sock = accept(mysock_lavagna, (struct sockaddr *)&utente, (socklen_t *)&utenteLen);
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

    // se ci sono almeno due utenti connessi mando la prima card in todo a tutti gli utenti
    if (utentiConnessi > 1)
        AVAILABLE_CARD();

    return;
}


void prepara_set() {
    max_des = 0; // Azzera il massimo all'inizio di ogni ciclo

    // 1. Aggiungo il descrittore del socket di ascolto
    FD_SET(mysock_lavagna, &readfds);
    if (mysock_lavagna > max_des) {
        max_des = mysock_lavagna;
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
        //printf("=============================\n");
    }
    else {
        printf("numero di utenti connessi: %d\n",utentiConnessi);
        while (utente_attuale != NULL) {
            printf("\t%d\n",utente_attuale->utente);
            utente_attuale = utente_attuale->prox;
        }
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
    switch (ID_comando) {
        case -1: {
            printf("Comando non riconosciuto.\n");
            break;
        }
        case 6: {
            SEND_USER_LIST();
            break;
        }
        case 7: {
            SHOW_LAVAGNA();
            break;
        }
        default: {
            printf("Comando non disponibile alla lavagna.\n");
        }
    }
}
/*
    #define ID_CREATE_CARD 1
    #define ID_HELLO 2
    #define ID_QUIT 3
    #define ID_ACK_CARD 4
    #define ID_CARD_DONE 5
    ///comandi lavagna
    #define ID_SEND_USER_LIST 6
    #define ID_SHOW_LAVAGNA 7
 */

void gestisci_messaggio_utente() {
    // scorro la lista degli utenti e per ognuno controllo se ha inviato un messaggio
    struct utente_des*attuale = testa_des_utente;
    while (!FD_ISSET(attuale->des,&readfds)) { // finché non trovo un descrittore nel set di descrittori con messaggi in entrata
        attuale = attuale->prox;
    }
    // sono sicuro che almeno un descrittore ci sia perché ho controllato prima
    int des_utente = attuale->des;
    int max_buf = DES_ATT_LEN+2*sizeof(uint32_t) + 1;
    char*buf = malloc(max_buf); // alloco un buffer dimensionato per la dimensione massima della descrizione attività

    int n = recv(des_utente,buf,max_buf,0); // ricevo il messaggio
    if (n == 0) { // l'utente ha terminato
        rimuovi_utente(attuale);
        return;
    }
    else if (n < 0) {
        perror("errore recv: ");
        exit(-1);
    }

    // controllo che comando è arrivato
    uint32_t id_comando;
    //id_comando = ntohl(*(uint32_t*)buf);
    memcpy(&id_comando, buf, sizeof(uint32_t));
    id_comando = ntohl(id_comando);
    if (DEBUG) {
        printf("ID Comando ricevuto: %d\n",id_comando);
    }

    if (id_comando < 1) {
        printf("comando ricevuto non disponibile.\n");
    }
    else if (id_comando == 0) { // l'utente ha terminato
        rimuovi_utente(attuale);
    }
    else if (id_comando == ID_CREATE_CARD) {
        crea_card(buf,attuale); // attuale è l'utente che ha inviato il messaggio
    }
    ////////// altri comandi...
}

void crea_card(char*buf,struct utente_des*utente) {
    if (DEBUG) {
        printf("creazione nuova card...\n");
    }
    // buf è il puntatore al messaggio ricevuto
    // il messaggio è strutturato come: ID_comando ID_carta descrizione_att
    card_t*new_card = malloc(sizeof(card_t)); // creo una nuova carta
    new_card->responsabile = 0;
    new_card->prox = NULL;
    size_t offset = sizeof(uint32_t);
    // ID della card
    //new_card->ID = *(uint32_t*)(buf+offset);
    uint32_t id_carta;
    memcpy(&id_carta,buf+offset,sizeof(uint32_t));
    id_carta = ntohl(id_carta);
    // controllo che ID carta non sia già stato usato
    if (check_id_card(id_carta)) {
        if (DEBUG) {
            printf("tentativo di aggiunta card con ID già utilizzato.\n");
        }
        // rispondi a utente
        send(utente->des, "1", 1, 0);
        return;
    }
    // id disponibile
    new_card->ID = id_carta;
    /*
    memcpy(&new_card->ID,buf+offset,sizeof(uint32_t));
    new_card->ID = ntohl(new_card->ID);
    */

    offset+= sizeof(uint32_t);

    // new_card->descrizione
    size_t str_len = strlen(buf+offset) + 1; // calcolo quanto è lunga la descrizione contenuta nel messaggio
    char*buf_des = malloc(str_len); // alloco zona di memoria per la descrizione
    memcpy(buf_des, buf+offset,str_len); // ricopio in buf_des la descrizione contenuta nel messaggio
    new_card->descrizione = buf_des; // new_card->descrizione è il puntatore alla zona di memoria dove è salvata la descrizione

    new_card->timestampUltimaModifica = time(NULL);
    free(buf);

    // adesso devo aggiungere la card in fondo alla colonna ToDo
    if (testaToDo == NULL) {
        testaToDo = new_card;
    }
    else {
        card_t*card_attuale = testaToDo;
        while (card_attuale->prox != NULL) {
            card_attuale = card_attuale->prox;
        }
        card_attuale->prox = new_card;
    }
    if (DEBUG) {
        printf("nuova card creata.\n");
    }
    send(utente->des, "0", 1, 0);
    SHOW_LAVAGNA();
}

void rimuovi_utente(struct utente_des*utente) {
    if (DEBUG) {
        printf("rimozione utente %d...\n", utente->utente);
    }
    // gestisci il caso in cui l'utente abbia una carta assegnata
    struct utente_des * old;
    struct utente_des * this;
    this = testa_des_utente;
    old = testa_des_utente;
    if (this == NULL) { // la lista è vuota
        printf("lista descrittori utenti vuota.\n");
        return;
    }
    if (this->utente == utente->utente) { // è il primo utente della lista
        testa_des_utente = testa_des_utente->prox;
    }
    else {
        while (this->utente != utente->utente) {
            if (this == NULL) {
                printf("descrittore utente non presente nella lista.\n");
                free(utente);
                return;
            }
            old = this;
            this = this->prox;
        }
    }
    // rimuovo this (che è utente a questo punto)
    old->prox = this->prox;
    printf("scollegato l'utente %d.\n", this->utente);
    free(this);
    utentiConnessi--;
    STAMPA_UTENTI();
}

int check_id_card(uint32_t ID) {
    card_t*attuale;
    for (int i = 0; i < 3; i++) {
        if (i == 0) {
            attuale = testaToDo;
        }
        else if (i == 1) {
            attuale = testaDoing;
        }
        else if (i == 2) {
            attuale = testaDone;
        }
        while (attuale != NULL) {
            if (attuale->ID == ID) { // ID già preso
                return 1;
            }
            attuale = attuale->prox;
        }
    }
    return 0;
}

int SEND_USER_LIST() {
    // controllo se ci sono utenti connessi
    if (!utentiConnessi) {
        printf("Non ci sono utenti connessi.\n");
        return -1;
    }
    // mando la lista degli utenti connessi ad ogni utente, se una comunicazione fallisce, inserisco il numero di porta relativo in @ref comunicazioni_fallite
    ///@todo

    return 0;
}

void AVAILABLE_CARD() {
    /// per ogni utente connesso:
        // in TCP: mando la prima carta in todo + (user list - destinatario) + numero utenti connessti, al destinatario
    /// attendo tutti gli ack (creo un vettore di utenti connessi, man mano che arrivano gli ack faccio la MOVE_CARD e aggiorno la lista)
    // ritorno ad ascoltare sulle porte
}
