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

#define DEBUG 1 // 1=stampa messaggi di debug , 0=non stampa messaggi di debug

#define PORTA_LAVAGNA 5678

#define LOCALHOST "127.0.0.1"

#define BACKLOG 3

#define DES_ATT_LEN 1024 // massimo numero di caratteri per la descrizione dell'attività

#define NUMERO_COMANDI_DISPONIBILI 7
#define MAX_LEN_COMANDO 14
/////////
/// comandi utente
#define ID_CREATE_CARD 1
#define ID_HELLO 2
#define ID_QUIT 3
#define ID_ACK_CARD 4
#define ID_CARD_DONE 5
///comandi lavagna
#define ID_SEND_USER_LIST 6
#define ID_SHOW_LAVAGNA 7
/////////

///////// typedef

typedef uint32_t utente_t;
typedef struct card  card_t;


////////// strutture

/* Struttura per le card
 *
 */
struct card {
    uint32_t ID;
    //int colonna; // 0 1 2 : ToDo Doing Done
    char *descrizione; // testo attività
    int responsabile; // n. di porta dell'utente che la implementa o la ha implementata
    time_t timestampUltimaModifica;
    card_t*prox;
};

////////////////////////////// funzioni


/* @brief Valida un comando inserito da terminale e ne restituisce il relativo ID
 * @param
 * buf : puntatore al buffer dove è salvata la stringa (terminata da \0, senza \n)
 */
int VALIDA_INPUT(const char*buf);
