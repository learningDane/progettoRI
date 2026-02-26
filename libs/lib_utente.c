#include "../include/include_utente.h"

int connesso = 0;
int mysock_utente;
fd_set readfds; // set di descrittori da controllare per la lettura

/// numero di card assegnate all'utente
int card_assegnate = 0;

/// puntatore alla testa della lista delle carte assegnate all'utente
card_t *testa_lista_card;

int rando() {
    srand(time(NULL));
    int n = rand();
    return n;
}

void ACK_CARD() {
    ///@todo
    // comunica alla lavagna la card che gli è stata assegnata
    // la card viene quindi spostata in doing
}

void CARD_DONE() {
    ///@todo
    // comunica alla lavagna la terminazione della card

    if (!connesso) {
        printf("comando non disponibile senza una connessione con la lavagna.\n");
        return;
    }
}

void QUIT () {
    // notifica uscita alla lavagna
    // se avevo carda in Doing, finisce in ToDo  (lo fa la lavagna)
    close(mysock_utente); // invia FIN alla lavagna
    exit(0);
}

void PONG_LAVAGNA() {
    ///@todo
    // contatta lavagna
}

void CREATE_CARD() {
    // comunico alla lavagna una nuova carta
    // serve ID, colonna, e testo attività

    if (!connesso) {
        printf("comando non disponibile senza una connessione con la lavagna.\n");
        return;
    }

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
        int received = recv(mysock_utente, buf_risposta, 1, 0);
        controlla_connessione(received);
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
    ///@todo
    // messaggio che si scambiano gli utenti, associando un costo all'esecuzione della card attualmente discussa
    // gli utenti convergono nell'assegnare la card all'utente che ha il costo minore

    // gli utenti comunicano UDP i propri costim ogni utente deve sapere il costo di tutti gli altri utenti
    // l'utente vincitore chiama ACK_CARD, gli altri ritornano
}

void HELLO(int*mysock_utente, struct sockaddr_in*lavagna) {
    if (connesso) {
        printf("connessione con la lavagna già effettuata.\n");
        return;
    }

    int err_connect = connect(*mysock_utente, (const struct sockaddr*)lavagna, (socklen_t)sizeof(*lavagna));
    if (err_connect) {
        perror("Errore connect: ");
        exit(-1);
    }

    // ricezione ACK da lavagna
    char buf_temp[3] = {0};
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

void controlla_connessione(int received) {
    if (received<=0) {
        printf("Lavagna disconnessa, terminazione...\n");
        exit(0);
    }
}

int rileva_input () {
    int n_des_pronti, max_des = 0;
    FD_ZERO(&readfds);

    // aggiungo il descrittore del socket connesso alla lavagna
    FD_SET(mysock_utente, &readfds);
    if (mysock_utente > max_des) {
        max_des = mysock_utente;
    }

    // aggiungo il descrittore di stdin
    FD_SET(STDIN_FILENO, &readfds);
    if (STDIN_FILENO > max_des) { // Di solito STDIN_FILENO è 0, ma è buona norma
        max_des = STDIN_FILENO;
    }

    n_des_pronti = select(max_des + 1, &readfds, NULL, NULL, /*timeout*/ NULL);

    // Controllo se ci sono errori
    if (n_des_pronti == -1) {
        perror("errore select():");
        QUIT();
    }

    // controllo se i descrittori pronti siano 0 (impossibile poichè non ho impostato un timeout)
    if (!n_des_pronti)
        return 0;

    // controllo se entrambi i descrittori siano pronti
    if (FD_ISSET(mysock_utente, &readfds) && FD_ISSET(STDIN_FILENO, &readfds))
        return 3;

    /// Controllo Socket
    if (FD_ISSET(mysock_utente, &readfds)) {
        return 1;
    }

    /// Controllo STDIN
    if (FD_ISSET(STDIN_FILENO, &readfds)) {
        return 2;
    }

    return 0;
}

void gestisci_messaggio_lavagna() {
    ///@todo
    // leggi dal socket
    // determina che comando è
    // agisci di conseguenza
}

void manda_comando_lavagna(int comando, const char* buf, const size_t length) {
    ///@todo
}

ssize_t recv_all(int sockfd, void *buf, size_t len) {
    size_t bytes_received = 0;
    char *ptr = (char *)buf;

    while (bytes_received < len) {
        ssize_t r = recv(sockfd, ptr + bytes_received, len - bytes_received, 0);
        if (r == 0) {
            // il server ha chiuso la connessione
            return 0;
        } else if (r < 0) {
            /// errore di rete
            perror("Errore in recv_all");
            return -1;
        }
        bytes_received += r;
    }
    return bytes_received; // Ritornerà sempre esattamente 'len'
}

void ricevi_available_card(int sock) {
    // 1. Leggo l'intestazione (comando + dimensione)
    uint32_t header[2];
    if (recv_all(sock, header, sizeof(header)) <= 0) {
        printf("Connessione col server persa.\n");
        exit(-1);
    }

    // Non serve estrarre l'ID comando se lo hai già fatto fuori,
    // qui facciamo finta di estrarre la lunghezza:
    uint32_t len = ntohl(header[1]);

    // 2. Alloco il buffer ESATTAMENTE della grandezza del payload
    char *payload = malloc(len);
    if (!payload) {
        perror("Errore malloc payload");
        exit(-1);
    }

    // 3. Leggo tutto il payload in modo sicuro
    if (recv_all(sock, payload, len) <= 0) {
        printf("Errore durante la ricezione della card.\n");
        free(payload);
        exit(-1);
    }

    // 4. "Spacchetto" il payload (ID Card + Descrizione)
    uint32_t id_carta;
    memcpy(&id_carta, payload, sizeof(uint32_t));
    id_carta = ntohl(id_carta);

    char *descrizione = payload + sizeof(uint32_t);

    printf("--- NUOVA CARD DISPONIBILE ---\n");
    printf("ID: %u\n", id_carta);
    printf("Descrizione: %s\n", descrizione);
    printf("------------------------------\n");

    // 5. Pulizia
    free(payload);
}
