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

/* @brief Controlla se ci sono nuovi messaggi dalla lavagna o dal terminale
 * @return
 * 0 se non ci sono messaggi,
 * 1 se vi è un messaggio dalla Lavagna,
 * 2 se vi è un messaggio dal terminale,
 * 3 se vi sono entrambi.
 */
int rileva_input();

/* @brief Gestisce il messaggio inviato dalla lavagna
 */
void gestisci_messaggio_lavagna();

/* @brief Si occupa di mandare un messaggio precostruito alla lavagna
 * @param comando è l'id del comando da mandare alla lavagna
 * @param buf è il puntatore al buffer da inviare
 * @param length è la lunghezza del buffer (NON del messaggio finale)
 */
void manda_comando_lavagna(int comando, const char* buf, const size_t length);

/* @brief Riceve dal socket indicato il numero di byte specificati e li inserisce in @ref buf
 *
 * È bloccante.
 * @param buf è il puntatore al buffer in cui scrivere i byte letti
 * @return il numero di byte letti, ovvero len, ritorna 0 se la lavagna ha chiuso il socket, -1 se vi è stato un errore di rete
 */
ssize_t recv_all(int sockfd, void *buf, size_t len);
