#include "include.h"

extern int utentiConnessi;
extern int mysock_lavagna; // mysock_lavagna è il socket di ascolto
extern int max_des;
extern char comando[256]; // buffer per il comando da terminale
extern uint16_t ID_LAVAGNA;
extern fd_set readfds; // set di descrittori da controllare per la lettura
extern card_t *testaToDo;
extern card_t *testaDoing;
extern card_t *testaDone;
extern char buf[MAX_LEN_COMANDO + 2]; // buffer per il comando da terminale, dimensionato per il comando più lungo possibile + \n + \0

extern struct utente_des * testa_des_utente;
extern struct sockaddr_in utente;
extern socklen_t utenteLen;

// struttura contentente porta utente, descrittore del relativo socket e puntatore per realizzare la lista
struct utente_des {
    // numero porta utente
    utente_t utente;
    // descrittore socket relativo all'utente
    int des;
    //puntatore al successivo utente
    struct utente_des*prox;
};
/*
 * @brief Mostra la lavagna a video
 *
 * Stampa la lavagna, con le colonne e le card assegnate nelle giuste colonne
 *
 * @param void
 */
void SHOW_LAVAGNA();

/* @brief Aggiunge al set di descrittori readfds ogni socket da controllare con select()
 *
 */
void prepara_set();

/* @brief Chiamato quando select rileva una nuova connessione sul socket che implementa la listen, aggiunge il nuovo utente alla lista degli utenti connessi
 *
 */
void accetta_utente();

/* @brief Restituisce il puntatore all'ultimo descrittore di socket per utente
 *
 * @param puntatore alla lista di descrittori da scorrere
 */
struct utente_des* scorri_lista (struct utente_des* des_attuale);

/* @brief Stampa la lista di utenti attualmente connessi
 *
 */
void STAMPA_UTENTI();

/* @brief Legge e gestisce l'input da terminale
 *
 */
void input_stdin();

/* @brief Gestisce un messaggio via socket da un utente tra quelli disponibili
 * @param void
 */
void gestisci_messaggio_utente();

/* @brief Rimuove l'utente passato come parametro dalla lista degli utenti connessi
 *
 */
void rimuovi_utente(struct utente_des*utente);

/* @brief Crea una nuova card sulla base del messaggio ricevuto
 * manda all'utente 1 se l'ID inserito non è disponibile, 0 altrimenti
 * @param char* , struct utente_des*
 */
void crea_card(char*buf,struct utente_des*utente);

/* @brief Controlla che l'ID fornito non sia già utilizzato da un'altra card
 * @return 0==ID disponibile, 1==ID non disponibile
 */
int check_id_card(uint32_t ID);

/* @brief La lavagna manda la lista delle porte utilizzate (gli utenti connessi) agli utenti.
 * @return -1 se non ci sono utenti connessi, -2 se la comunicazione è fallita, 0 se va tutto bene
 */
int SEND_USER_LIST();

/* @brief Manda la prima card in ToDo, e la lista di utenti connessi (meno il destinatario) a tutti gli utenti connessi.
 *
 */
void AVAILABLE_CARD();

/* @brief restituisce il puntatore al descrittore dell'utente specificato
 * @param utente è l'id dell'utente da cercare nella lista degli utenti connessi
 * @param NULLABLE prior è il puntatore a struct utente_des (va passato per riferimento) dove scrivere il puntatore al descrittore che precede quello richiesto (utile per operazioni di rimozione utenti)
 * @return il puntatore (struct des_utente*) al descrittore dell'utente richiesto, NULL se non è stato trovato
 */
utente_des_t* trova_utente(utente_t utente, utente_des_t**prior);

/* @brief Invia una matrice ad un utente
 * Si occupa di portare in endianess di rete
 * @param utente è il puntatore al descrittore di utente destinatario
 * @param id_comando
 * @param mat è il puntatore alla locazione di memoria che contiene la matrice di interi da mandare
 * @param len è il numero di interi
 * @return int: 0 se tutto è andato bene, -1 altrimenti
 */
int manda_matrice(utente_des_t*utente, uint32_t*mat, int len);

/* @brief Inserisce la card desiderata in fondo alla lista scelta
 * @param testaLista va passato per riferimento
 */
void inserimento_lista_card(card_t**testaLista, card_t*newCard);

/* @brief Estrae la prima card nella lista specificata
 * @param testaLista va passato per riferimento
 * Si occupa di "scollegare" la card estratta dalla lista di origine
 * @return Il puntatore alla card estratta
 */
card_t* estrazione_lista_card(card_t**testaLista);

/* @brief Sposta la card indicata dalla lista in cui è a quella desiderata
 *
 * in particolare esegue estrazione_lista_card e inserimento_lista_card
 * @param listaSorg va passato per riferimento
 * @param listaDest va passato per riferimento
 */
void MOVE_CARD(card_t*card, card_t**listaDest, card_t**listaSorg);

/* @brief Legge dal socket dell'utente l'id della card da mettere in Doing
 *
 * In particolare esegue MOVE_CARD(carta, testaDoing)
 */
void ACK_CARD(utente_des_t*utente);

/* @brief restituisce il puntatore al descrittore di card specificata
* @param lista è la lista nella quale trovare la card
* @param ID è l'id della card da cercare nella lista indicata
* @param NULLABLE prior è il puntatore a card_t (va passato per riferimento) dove scrivere il puntatore al descrittore che precede quello richiesto (utile per operazioni di rimozione card)
* @return il puntatore (card_t*) al descrittore della card richiesta, NULL se non è stata trovata
 */
card_t* trova_card(card_t*lista, uint32_t ID, card_t**prior);
