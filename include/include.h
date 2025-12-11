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
#define DEBUG 1 // 1=stampa messaggi di debug , 0=non stampa messaggi di debug
#define NUMERO_COMANDI_DISPONIBILI 7
#define MAX_LEN_COMANDO 14

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


/* @brief Valida un comando inserito da terminale e ne restituisce il relativo ID
 * @param
 * buf : puntatore al buffer dove è salvata la stringa (terminata da \0, senza \n)
 */
int VALIDA_INPUT(const char*buf);
