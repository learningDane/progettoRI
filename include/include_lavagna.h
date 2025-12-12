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
