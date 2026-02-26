#include "include/include_lavagna.h"

int main() {

  // 1. mostra a video lo stato della lavagna
  SHOW_LAVAGNA();

  // ad ogni spostamento o creazione di una card, mostra la lavagna

  // se una card è in Doing da troppo tempo (1minuto.30s), ping utente, se non
  // riceve risposta dopo 30 secondi suppone che si sia disconnesso

  // se ci sono almeno due utenti viene mandato a tutti AVAILABLE_CARD.
  // Questa operazione verrà ripetuta ogni volta che un utente ha terminato una
  // card e ci sono almeno due utenti presenti e card disponibili.

  //////////////////////////////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////inizializzazione socket di ascolto nuove
  ///connessioni
  int port = PORTA_LAVAGNA;
  printf("porta da collegare: %d\n", port);

  // accettazione connessioni TCP dagli utenti tramite socket non bloccanti e
  // I/O multiplexing connessione TCP bloccante

  struct sockaddr_in my_addr;
  // struct sockaddr_in utente;
  // int utenteLen = sizeof(utente);
  memset(&my_addr, 0, (size_t)sizeof(my_addr));
  my_addr.sin_family = AF_INET;
  my_addr.sin_port = port;
  inet_pton(AF_INET, LOCALHOST, &my_addr.sin_addr);

  mysock_lavagna = socket(AF_INET, SOCK_STREAM, 0);
  printf("Aperto socket %d\n", mysock_lavagna);

  int err_bind = bind(mysock_lavagna, (struct sockaddr *)&my_addr,
                      (socklen_t)sizeof(my_addr));
  if (err_bind) {
    perror("errore su bind: ");
    return -1;
  }
  printf("Socket collegato\n");

  int err_listen = listen(mysock_lavagna, BACKLOG);
  if (err_listen) {
    perror("errore su listen: ");
    return -1;
  }
  printf("socket in ascolto per nuove connessioni...\n");
  ////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////////////////////////////

  int des_pronti = 0;

  while (1) {
loop:
    FD_ZERO(&readfds);
    prepara_set();
    DBG(Controllo socket con select()...);
    des_pronti = select(max_des + 1, &readfds, NULL, NULL, /*timeout*/ NULL);

    if (DEBUG) {
      printf("numero di descrittori pronti: %d\n", des_pronti);
    }

    if (des_pronti == -1) {
      printf("errore select\n");
      exit(-1);
    }

    // 1. controllo input da terminale
    if (FD_ISSET(STDIN_FILENO, &readfds)) {
      if (DEBUG) {
        printf("gestisco input da terminale\n");
      }
      // leggi il comando dalla riga di comando
      /*
      if (fgets(comando, sizeof(comando), stdin) != NULL) {
          // Processa il comando letto
          //handle_terminal_input(comando);
      }
       */
      input_stdin();
      goto loop;
    }

    // 2. Controllo nuove connessioni
    if (FD_ISSET(mysock_lavagna, &readfds)) {
      if (DEBUG) {
        printf("gestisco nuove connessioni\n");
      }
      accetta_utente();
    }

    // 3. controlla connessioni esistenti
    else if (des_pronti >
             0) { // qualche descrittore è pronto, ma non è ne stdin ne
                  // mysock_lavagna, quindi è una connessione esistente
      if (DEBUG) {
        printf("gestisco connessioni esistenti\n");
        gestisci_messaggio_utente();
      }
    }
  }
}

/*
    la lavagna è identificata da:
        - ID
        - colonne
        - card
    le card sono almeno 10
    ogni card è identificata da:
        - ID
        - colonna
        - testo attività
        - utente che la sta facendo oppure utente che l'ha fatta
        - timestamp dell'ultima modifica
    gli utenti sono identificati dal loro numero di porta (incrementale a
   partire dalla 5679) per prima cosa l'utente contatta la lavagna e si registra
*/
