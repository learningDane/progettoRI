#include "include.h"

extern int connesso; // 0=non connesso alla lavagna , 1=connesso alla lavagna
extern int mysock_utente; // socket connesso alla lavagna

/* @brief Generare un numero randomico secondo le specifiche fornite
 *
 */
int rando();

/*
 * @brief Comunica alla lavagna la card che gli è stata assegnata
 *
 * @param
 */
void ACK_CARD();

/*
 * @brief Comunica alla lavagna la terminazione dell'attività assegnata
 *
 * @param
 */
void CARD_DONE();

/*
 * @brief Comunico alla lavagna una nuova carta
 *
 * Serve ID e testo attività
 */
void CREATE_CARD();

/*
 * @brief Notifica uscita alla lavagna
 *
 * Se aveva una carta in Doing, finisce in ToDo
 *
 * @param
 */
void QUIT ();

/*
 * @brief Risponde al PING_UTENTE
 *
 * @param
 */
void PONG_LAVAGNA();

/*
 * @brief Assegnamento della carta discussa tramite consenso
 *
 * Messaggio che si scambiano gli utenti, associando un costo all'esecuzione della card attualmente discussa gli utenti convergono nell'assegnare la card all'utente che ha il costo minore
 *
 * @param
 */
void CHOOSE_USER();

/*
 * @brief La lavagna sposta la card nella colonna corretta
 *
 * @param
 */
void MOVE_CARD();

/*
 * @brief Registrazione
 *
 *  Lavagna risponde con card in ToDo
 *
 * @param
 */
void HELLO(int*mysock_utente, struct sockaddr_in*lavagna);

/* @brief Controlla se la connessione è ancora valida.
 * @param received è l'intero restituito da una precedente read()
 */
void controlla_connessione(int received);
