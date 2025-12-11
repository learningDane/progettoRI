#include "../include/include_utente.h"

int connesso = 0; // 0=non connesso alla lavagna , 1=connesso alla lavagna

int rando() {
    srand(time(NULL));
    int n = rand();
    return n;
}

void ACK_CARD() {
    // comunica alla lavagna la card che gli è stata assegnata
}

void CARD_DONE() {
    // comunica alla lavagna la terminazione della card
}

void QUIT () {
    // notifica uscita alla lavagna
    // se avevo carda in Doing, finisce in ToDo
}

void PONG_LAVAGNA() {
    // contatta lavagna
}

void CREATE_CARD() {
    // comunico alla lavagna una nuova carta
    // serve ID, colonna, e testo attività
}

void CHOOSE_USER() {
    // messaggio che si scambiano gli utenti, associando un costo all'esecuzione della card attualmente discussa
    // gli utenti convergono nell'assegnare la card all'utente che ha il costo minore
}

void MOVE_CARD() {
    // sposta la card nella colonna corretta

    // infine mostra la lavagna
    //SHOW_LAVAGNA();
}

void HELLO(int*mysock, struct sockaddr_in*lavagna) {
    if (connesso) {
        printf("connessione con la lavagna già effettuata.\n");
        return;
    }
    // lavagna risponde con card in ToDo
    int err_connect = connect(*mysock, (const struct sockaddr*)lavagna, (socklen_t)sizeof(*lavagna));
    if (err_connect) {
        perror("Errore connect: ");
        exit(-1);
    }

    // ricezione ACK da lavagna
    char buf_temp[2];
    recv(*mysock, buf_temp, 2, 0); // recv() NON aggiunge '\0'
    if (strcmp(buf_temp,"OK") == 0) {
        if (DEBUG) {
            printf("Connesso alla lavagna\n");
            connesso = 1;
        }
    }
    else {
        printf("ACK non ricevuto.\n");
        printf("messaggio ricevuto: %s\n", buf_temp);
        exit(-1);
    }
}
