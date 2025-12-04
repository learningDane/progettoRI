// file include
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <stdio.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <sys/select.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
//#include <sys/types.h>

#define PORTA_LAVAGNA 5678
#define LOCALHOST "127.0.0.1"
#define BACKLOG 3
#define DEBUG 1

// typedef
typedef int utente_t;
typedef struct card card_t;

// strutture

/* Struttura per le card
 *
 */
struct card {
    uint16_t ID;
    //int colonna; // 0 1 2 : ToDo Doing Done
    char *descrizione; // testo attività
    int responsabile; // n. di porta dell'utente che la implementa o la ha implementata
    time_t timestampUltimaModifica;
};

////////////////////////////// funzioni


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
 * @brief Comunico alla lavagna una nuova carta
 *
 * Serve ID, colonna, e testo attività
 *
 * @param
 */
void CREATE_CARD(card_t nuovaCarta);

/*
 * @brief Assegnamento della carta discussa tramite consenso
 *
 * Messaggio che si scambiano gli utenti, associando un costo all'esecuzione della card attualmente discussa gli utenti convergono nell'assegnare la card all'utente che ha il costo minore
 *
 * @param
 */
void CHOOSE_USER();

/*
 * @brief Comunica alla lavagna la card che gli è stata assegnata
 *
 * @param
 */
void ACK_CARD(card_t cartaAssegnata);

/*
 * @brief Comunica alla lavagna la terminazione dell'attività assegnata
 *
 * @param
 */
void CARD_DONE();

/*
 * @brief La lavagna sposta la card nella colonna corretta
 *
 * @param
 */
void MOVE_CARD(card_t cartaDaSpostare);

/*
 * @brief Manda lista delle porte degli utenti
 *
 * @param
 */
void SEND_USER_LIST();

/*
 * @brief Pinga un utente dopo un timeout
 *
 * Restituisce 0 se l'utente risponde (entro 30 secondi), -1 altrimenti
 * @param utente_t utente
 */
int PING_USER(utente_t utente);

/*
 * @brief Invia la card da assegnare
 *
 * Quando c'è più di un utente registrato, la lavagna invia a tutti gli utenti connessi la prima card in ToDo.
 * Oltre alla card include la lista delle porte degli utenti presenti (escluso il destinatario), e il numero degli utenti presenti
 *
 * @param
 */
void AVAILABLE_CARD(card_t cartaDisponibile);
