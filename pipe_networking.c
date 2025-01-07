#include "pipe_networking.h"
#include <sys/types.h>
#include <sys/stat.h>

//UPSTREAM = to the server / from the client
//DOWNSTREAM = to the client / from the server
/*=========================
  server_setup

  creates the WKP and opens it, waiting for a  connection.
  removes the WKP once a connection has been made

  returns the file descriptor for the upstream pipe.
  =========================*/
int server_setup() {
  int from_client;
  if(mkfifo(WKP, 0666)){
    perror("fail WKP creation");
    exit(1);
  }
  from_client = open(WKP, O_RDONLY);
  if(from_client == -1){
    perror("fail open WKP");
    exit(1);
  }
  remove(WKP);

  return from_client;
}

/*=========================
  server_handshake
  args: int * to_client

  Performs the server side pipe 3 way handshake.
  Sets *to_client to the file descriptor to the downstream pipe (Client's private pipe).

  returns the file descriptor for the upstream pipe (see server setup).
  =========================*/
int server_handshake(int *to_client) {
  char clientPipe[HANDSHAKE_BUFFER_SIZE];
  int from_client;
  int rand = rand();
  int ack;

  from_client = open(WKP, O_RDONLY);
  read(from_client, clientPipe, HANDSHAKE_BUFFER_SIZE);
  close(from_client);

  *to_client = open(clientPipe, O_WRONLY);
  write(*to_client, &rand, sizeof(rand));
  read(from_client, &ack, sizeof(int));

  if(ack != rand + 1){
    perror("handshake failed, ack invalid");
    exit(1);
  }

  return from_client;
}


/*=========================
  client_handshake
  args: int * to_server

  Performs the client side pipe 3 way handshake.
  Sets *to_server to the file descriptor for the upstream pipe.

  returns the file descriptor for the downstream pipe.
  =========================*/
int client_handshake(int *to_server) {
  int from_server;
  return from_server;
}


/*=========================
  server_connect
  args: int from_client

  handles the subserver portion of the 3 way handshake

  returns the file descriptor for the downstream pipe.
  =========================*/
int server_connect(int from_client) {
  int to_client  = 0;
  return to_client;
}
