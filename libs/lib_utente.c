#include "../include/include_utente.h"

int connesso = 0;
int mysock_utente;

int rando() {
    srand(time(NULL));
    int n = rand();
    return n;
}

void ACK_CARD() {
    // comunica alla lavagna la card che gli è stata assegnata
}

void CARD_DONE() {
    // comunica alla lavagna la terminazione della card
}

void QUIT () {
    // notifica uscita alla lavagna
    // se avevo carda in Doing, finisce in ToDo  (lo fa la lavagna)
    close(mysock_utente); // invia FIN alla lavagna
    exit(0);
}

void PONG_LAVAGNA() {
    // contatta lavagna
}

void CREATE_CARD() {
    // comunico alla lavagna una nuova carta
    // serve ID, colonna, e testo attività
in_id:
    printf("Inserire ID carta:\n");
    char buf_id[13] = {0};
    fgets(buf_id, 12, stdin);
    for (int i = 0; i < 13; i++) {
        if (buf_id[i] != 0 && i == 9) {
            if (atoi(buf_id)  > 1111111111) {
                printf("Numero troppo grande.\n");
                goto in_id;
            }
        }
        if (buf_id[i] == '\n') {
            buf_id[i] = '\0';
            break;
        }
        if (buf_id[i] < '0' || buf_id[i] > '9') {
            printf("non è un numero, oppure il numero inserito è troppo grande, ritentare...\n");
            goto in_id;
        }
    }
    int id_carta = atoi(buf_id);
    if (DEBUG) {
        printf("numero inserito: %d\n", id_carta);
    }

    ////////////////////////

    printf("Inserire descrizione attività:\n");
    char buf_des[DES_ATT_LEN];

    if (fgets(buf_des, DES_ATT_LEN, stdin) == NULL) return;
    size_t des_len = strlen(buf_des);
    if (des_len > 0 && buf_des[des_len - 1] == '\n') { // rimuovo il newline
        buf_des[des_len - 1] = '\0';
    }

    size_t total_len = sizeof(uint32_t) + sizeof(uint32_t) + strlen(buf_des) + 1; // la dimensione del messaggio è 2x int (uint_32) + strlen + '\0'
    char* buf = (char*)malloc(total_len);

    if (buf == NULL) { // controllo errore nella malloc
        perror("malloc error");
        return;
    }

    size_t offset = 0;

    uint32_t net_command_id = htonl(ID_CREATE_CARD); // porto in endianess di rete
    uint32_t net_id_carta = htonl(id_carta);

    memcpy(buf + offset, &net_command_id, sizeof(uint32_t)); // primo elemento = ID del comando
    offset += sizeof(uint32_t);

    memcpy(buf + offset, &net_id_carta, sizeof(uint32_t)); // secondo elemento = ID della carta
    offset += sizeof(uint32_t);

    memcpy(buf + offset, buf_des, strlen(buf_des) + 1 /*per via del '\0'*/); // terzo elemento = descrizione attività

    if (send(mysock_utente, buf, total_len, 0) < 0) { // invio il messaggio completo alla lavagna
        perror("send error");
    } else {
        printf("Richiesta CREATE_CARD inviata. Totale %zu bytes.\n", total_len);
        char buf_risposta[2] = {0};
        recv(mysock_utente, buf_risposta, 1, 0);
        // CONTROLLA BUF_RISPOSTA, 1=ID non disponibile, 0=ID disponibile
        buf_risposta[1] = '\0';
        if (strcmp("0", buf_risposta) == 0) { // se ricevo 0 da lavagna, ID è disponibile
            printf("card accettata.\n");
        }
        else { // lavagna invia 1: ID non disponibile
            printf("ID card non disponibile, ritentare.\n");
        }
    }

    free(buf);
}

void CHOOSE_USER() {
    // messaggio che si scambiano gli utenti, associando un costo all'esecuzione della card attualmente discussa
    // gli utenti convergono nell'assegnare la card all'utente che ha il costo minore
}

void MOVE_CARD() {
    // sposta la card nella colonna corretta

    // infine mostra la lavagna
    //SHOW_LAVAGNA();
}

void HELLO(int*mysock_utente, struct sockaddr_in*lavagna) {
    if (connesso) {
        printf("connessione con la lavagna già effettuata.\n");
        return;
    }
    // lavagna risponde con card in ToDo
    int err_connect = connect(*mysock_utente, (const struct sockaddr*)lavagna, (socklen_t)sizeof(*lavagna));
    if (err_connect) {
        perror("Errore connect: ");
        exit(-1);
    }

    // ricezione ACK da lavagna
    char buf_temp[2];
    recv(*mysock_utente, buf_temp, 2, 0); // recv() NON aggiunge '\0'
    if (strcmp(buf_temp,"OK") == 0) {
        if (DEBUG) {
            printf("Connesso alla lavagna\n");
            connesso = 1;
        }
    }
    else {
        printf("ACK non ricevuto.\n");
        printf("messaggio ricevuto: %s\n", buf_temp);
        exit(-1);
    }
}
