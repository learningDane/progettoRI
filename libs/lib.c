#include "../include/include.h"

// 0=utente 1=lavagna
int selettore;

void QUIT () {
    // notifica uscita alla lavagna
    // se avevo carda in Doing, finisce in ToDo
}

void PONG_LAVAGNA() {
    // contatta lavagna
}

void CREATE_CARD(card_t nuovaCarta) {
    // comunico alla lavagna una nuova carta
    // serve ID, colonna, e testo attività
}

void CHOOSE_USER() {
    // messaggio che si scambiano gli utenti, associando un costo all'esecuzione della card attualmente discussa
    // gli utenti convergono nell'assegnare la card all'utente che ha il costo minore
}

void ACK_CARD(card_t cartaAssegnata) {
    // comunica alla lavagna la card che gli è stata assegnata
}

void CARD_DONE() {
    // comunica alla lavagna la terminazione della card
}

void MOVE_CARD(card_t cartaDaSpostare) {
    // sposta la card nella colonna corretta

    // infine mostra la lavagna
    //SHOW_LAVAGNA();
}

void SEND_USER_LIST() {
    // manda lista delle porte degli utenti
}

int PING_USER(utente_t utente) {
    return 0; // 0 se utente risponde, -1 altrimenti
}

void AVAILABLE_CARD(card_t cartaDisponibile) {
    // quando c'è più di un utente registrato, la lavagna invia a tutti gli utenti connessi la prima card in ToDo.
    // oltre alla card include la lista delle porte degli utenti presenti (escluso il destinatario), e il numero degli utenti presenti

    // quando un utente riceve un available card: geenera un "n" casuale
}
