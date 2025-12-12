#include "../include/include.h"
#include <stdio.h>
#include <string.h>

// 0=utente 1=lavagna
// int selettore;

const char *comandi[] = { // numero di comandi disponibili (NUMERO_COMANDI_CORRETTI in include/include.h)
    "CREATE_CARD", // solo utente 1
    "HELLO", // solo utente 2
    "QUIT", // solo utente 3
    "ACK_CARD", // solo utente 4
    "CARD_DONE", // solo utente 5

    "SEND_USER_LIST", // solo lavagna 6
    "SHOW_LAVAGNA" // solo lavagna 7

    // il comando più lungo è di 14 caratteri (tieni di conto in più di \n e \0)
    // MAX_LEN_COMANDO in include/include.h
};


int VALIDA_INPUT(const char*buf) {
    // controlla che la stringa contenuta in buf sia contenuta in comandi[]
    // la stringa deve essere priva di \n
    for (int i = 0; i < NUMERO_COMANDI_DISPONIBILI; i++) {
        if (strcmp(buf,comandi[i]) == 0) {
            if (DEBUG) {
                printf("corrispondenza comando trovata, ID = %d\n",i+1);
            }
            return i+1;
        }
    }
    return -1;
    // fgets include il \n se presente e aggiunge \0
}
