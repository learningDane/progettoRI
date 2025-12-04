#include "include.h"


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
