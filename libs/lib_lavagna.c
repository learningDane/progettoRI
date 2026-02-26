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

/// Puntatore alla testa della lista degli utenti connessi
utente_des_t * testa_des_utente = NULL;

card_t *testaPendingCards = NULL;

struct sockaddr_in utente;
socklen_t utenteLen = sizeof(utente);

int attesa_ack = 0;


void SHOW_LAVAGNA() {
    // stampa la lavagna, con le colonne e le card assegnate nelle giuste colonne
    printf("\n");
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
    printf("\n");
}


utente_des_t* scorri_lista (utente_des_t* des_attuale) {
    if (des_attuale == NULL) {
        // gestisci errore
        return NULL;
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
    utente_des_t*nuovo_utente = malloc(sizeof(utente_des_t));
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
        utente_des_t*ultimo_utente = scorri_lista(testa_des_utente);
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
    utente_des_t *des_attuale = testa_des_utente;
    while (des_attuale != NULL) {
        FD_SET(des_attuale->des, &readfds);
        if (des_attuale->des > max_des) {
            max_des = des_attuale->des;
        }
        des_attuale = des_attuale->prox;
    }
}

void STAMPA_UTENTI() {
    printf("\n");
    printf("====== Utenti Connessi ======\n");
    utente_des_t*utente_attuale = testa_des_utente;
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
    printf("\n");
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
        case 3: {
            printf("terminazione...\n");
            exit(0);
        }
        case 6: {
            //SEND_USER_LIST();
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
    #define ID_AVAILABLE_CARD 8
 */

void gestisci_messaggio_utente() {
    // scorro la lista degli utenti e per ognuno controllo se ha inviato un messaggio
    utente_des_t*attuale = testa_des_utente;
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
        free(buf);
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
    free(buf);
}

void crea_card(char*buf,utente_des_t*utente) {
    if (DEBUG) {
        printf("creazione nuova card...\n");
    }
    // buf è il puntatore al messaggio ricevuto
    // il messaggio è strutturato come: ID_comando ID_carta descrizione_att
    card_t*new_card = malloc(sizeof(card_t)); // creo una nuova carta
    new_card->responsabile = 0;
    new_card->timestampUltimaModifica = time(NULL);
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

void rimuovi_utente(utente_des_t*utente) {
    if (DEBUG) {
        printf("rimozione utente %d...\n", utente->utente);
    }
    // gestisci il caso in cui l'utente abbia una carta assegnata
    utente_des_t * old;
    utente_des_t * this;
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
        while (this != NULL && this->utente != utente->utente) {

            old = this;
            this = this->prox;
        }
        if (this == NULL) {
            printf("descrittore utente non presente nella lista.\n");
            return;
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

    // manda comando+dimensione matrice
    uint32_t mes1[2];
    mes1[0] = htonl(ID_SEND_USER_LIST);
    mes1[1] = htonl(utentiConnessi);

    // creo una matrice sizeof(int)*utentiConnessi
    uint32_t utenti[utentiConnessi];
    int i = 0;
    utente_des_t*lavoro = testa_des_utente;
    while (lavoro != NULL) {
        utenti[i] = lavoro->utente;
        lavoro = lavoro->prox;
        i++;
    }

    lavoro = testa_des_utente;
    while (lavoro != NULL) {
        // mando comando e dimensione matrice
        ssize_t err = send(lavoro->des, mes1, sizeof(mes1), 0);
        if (err < 0) {
            perror("SEND_USER_LIST: errore send:");
            /*provvisorio*/ return -2;
            // gestire errore ?
            // dovrei comunicare a ogni utente che la lista non è affidabile e l'assegnazione della card va annullata @problema
        }
        err = manda_matrice(lavoro, utenti, utentiConnessi);
        if (err < 0) {
            perror("SEND_USER_LIST: errore send:");
            /*provvisorio*/ return -2;
        }
        lavoro = lavoro->prox;
    }
    return 0;
}

void AVAILABLE_CARD() {
    if (attesa_ack)
        return;
    attesa_ack = 1;
    /// ad ogni utente connesso mando la prima carta in todo
    if (SEND_USER_LIST() < 0) {
        DBG(Errore SEND_USER_LIST);
        exit(-1);
    }
    // scelgo la prima card
    if (!testaToDo) {
        attesa_ack = 0;
        return;
    }
    card_t*card = estrazione_lista_card(&testaToDo);
    inserimento_lista_card(&testaPendingCards,card);

    // creo messaggio: ID_CARD + descrizione
    char*buf = malloc(sizeof(uint32_t) + strlen(card->descrizione) + 1);
    uint32_t*tmp_int = (uint32_t*)buf;
    *tmp_int = htonl(card->ID);
    char*tmp = buf + sizeof(uint32_t);
    strcpy((char*)tmp,(const char*)card->descrizione); // include '\0'
    size_t len = sizeof(uint32_t) + strlen(card->descrizione) + 1; // lunghezza di @ref buf in byte

    // invio ID_comando + lunghezza messaggio
    uint32_t mes1[2];
    mes1[0] = htonl(ID_AVAILABLE_CARD);
    mes1[1] = htonl(len);

    utente_des_t*lavoro = testa_des_utente;
    while (lavoro != NULL) {
        // ad ogni utente mando mes1 e buf
        send(lavoro->des,mes1, sizeof(mes1), 0); // mando comando e dimensione prox messaggio
        {
            size_t sent = 0, total = len;
            while (sent < total) {
                ssize_t sent_now = send(lavoro->des, buf + sent, len - sent, 0);
                if (sent_now < 0) {
                    perror("AVAILABLE_CARD: errore send():");
                    exit(-1);
                }
                sent += sent_now;
            }
        }
        lavoro = lavoro->prox;
    }
    // finito di mandare i messaggi
    free(buf);
    return;
}

utente_des_t* trova_utente(utente_t utente, utente_des_t **prior) {
    utente_des_t *lavoro = testa_des_utente;
    utente_des_t *prev = NULL;

    while (lavoro != NULL && lavoro->utente != utente) {
        prev = lavoro;
        lavoro = lavoro->prox;
    }

    if (prior)
        *prior = prev;

    return lavoro;   // NULL se non è stato trovato
}

int manda_matrice(utente_des_t*utente, uint32_t*mat, int len) {
    uint32_t *mat_msg = malloc(len * sizeof(uint32_t));
    if (!mat_msg) {
        perror("manda_matrice(): Errore Malloc:");
        exit(-1);
    }
    for (uint32_t i = 0; i < len; i++) {
        mat_msg[i] = htonl(mat[i]);
    }
    size_t bytes_sent = 0;
    size_t total_bytes = len*(sizeof(uint32_t));
    while (bytes_sent < total_bytes) {
        ssize_t sent = send(utente->des, (char*)mat_msg + bytes_sent, total_bytes - bytes_sent, 0);

        if (sent < 0) {
            perror("Error sending matrix");
            free(mat_msg);
            return -1; // Send failed
        }
        bytes_sent += sent;
    }
    free(mat_msg);
    return 0;
}

void inserimento_lista_card(card_t**testaLista, card_t*newCard) {
    newCard->prox = NULL;
    card_t*lavoro = *testaLista;
    if (!*testaLista) {
        *testaLista = newCard;
        return;
    }
    while (lavoro->prox) {
        lavoro = lavoro->prox;
    }
    lavoro->prox = newCard;
}

card_t* estrazione_lista_card(card_t**testaLista) {
    if (!*testaLista)
        return NULL;
    // estraggo la card in testa
    card_t*card_estratta = *testaLista;
    *testaLista = (*testaLista)->prox;
    card_estratta->prox = NULL; // scollego la card estratta dal resto della lista
    return card_estratta;
}

void MOVE_CARD(card_t*card, card_t**listaDest, card_t**listaSorg) {
    card_t*prior;
    card_t*lavoro = trova_card(*listaSorg, card->ID, &prior);
    if (!lavoro) {
        DBG(MOVE_CARD: CARD NON TROVATA IN QUESTA LISTA);
        exit(-1);
    }
    if (!prior) {
        *listaSorg = lavoro->prox;
    }
    else {
        prior->prox = lavoro->prox;
    }
    lavoro->prox = NULL;

    inserimento_lista_card(listaDest, lavoro);
}

void ACK_CARD(utente_des_t*utente) {
    ///@todo
    uint32_t buf;
    ssize_t n = recv(utente->des, &buf, sizeof(uint32_t),0);
    if (n <= 0) {
        DBG(ACK_CARD: utente disconnesso oppurre errore di rete.);
        exit(-1);
    }
    uint32_t ID = ntohl(buf); /// ID della card da spostare
    if (DEBUG) {
        printf("Ricevuto ACK per card %u da parte di utente %u.\n", ID, utente->utente);
    }
    card_t*prior = NULL;
    card_t*card_accettata = trova_card(testaPendingCards,ID,&prior);
    if (!card_accettata) {
        DBG(ACK_CARD: ricevuto ACK per card non in pending);
        exit(-1);
    }
    card_accettata->responsabile = utente->utente;
    card_accettata->timestampUltimaModifica = time(NULL);
    MOVE_CARD(card_accettata, &testaDoing, &testaPendingCards);

    attesa_ack = 0;
    SHOW_LAVAGNA();
    if (testaToDo) {
        // ci sono altre card in todo, le invio
        AVAILABLE_CARD();
    }
    return;
}

card_t* trova_card(card_t*lista, uint32_t ID, card_t**prior) {
    card_t *lavoro = lista;
    card_t *prev = NULL;

    while (lavoro != NULL && lavoro->ID != ID) {
        prev = lavoro;
        lavoro = lavoro->prox;
    }

    if (prior)
        *prior = prev;

    return lavoro;   // NULL se non è stato trovato
}
