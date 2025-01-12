#include "pipe_networking.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>

//UPSTREAM = to the server / from the client
//DOWNSTREAM = to the client / from the server
/*=========================
  server_setup

  creates the WKP and opens it, waiting for a  connection.
  removes the WKP once a connection has been made

  returns the file descriptor for the upstream pipe.
  =========================*/
int server_setup() {
  signal(SIGINT, cleanWKP);
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
  int random = rand();
  int ack;

  printf("[server] creating WKP...\n");
  if(mkfifo(WKP, 0666) == -1){
    perror("fail WKP creation");
    exit(1);
  }
  
  from_client = open(WKP, O_RDONLY);
  if(from_client == -1){
    perror("failed open wkp");
    remove(WKP);
    exit(1);
  }

  if(read(from_client, clientPipe, HANDSHAKE_BUFFER_SIZE) <= 0){
    perror("failed to read pipe");
    close(from_client);
    remove(WKP);
    exit(1);
  }  
  printf("[server] received pipe name: %s\n", clientPipe);
  remove(WKP);
  printf("[server] WKP removed\n");

  *to_client = open(clientPipe, O_WRONLY);
  if(*to_client == -1){
    perror("fail open client pipe");
    close(from_client);
    exit(1);
  }
  printf("[server] connected to client's pipe\n");

  if(write(*to_client, &random, sizeof(random)) == -1){
    perror("failed write synack");
    close(from_client);
    close(*to_client);
    exit(1);
  }
  
  if(read(from_client, &ack, sizeof(int)) == 0){
    perror("failed to read ack");
    close(from_client);
    close(*to_client);
    exit(1);    
  }

  if(ack != random + 1){
    perror("handshake failed, ack invalid");
    close(from_client);
    close(*to_client);
    exit(1);
  }

  printf("[server] handshake complete\n");
  close(from_client);

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
  char clientPipe[HANDSHAKE_BUFFER_SIZE];
  int random = rand();
  int ack;

  sprintf(clientPipe, "%d", getpid());
  printf("[client] creating pipe: %s\n", clientPipe);
  if(mkfifo(clientPipe, 0666) == -1){
    perror("failed create pipe");
    exit(1);
  }

  printf("[client] connecting to WKP...\n");
  *to_server = open(WKP, O_WRONLY);
  if(*to_server == -1){
    perror("[client] failed open WKP");
    remove(clientPipe);
    exit(1);
  }

  if(write(*to_server, clientPipe, HANDSHAKE_BUFFER_SIZE) == -1){
    perror("failed send pipe");
    close(*to_server);
    remove(clientPipe);
    exit(1);
  }

  printf("[client] connecting to pipe...\n");
  from_server = open(clientPipe, O_RDONLY);
  if(from_server ==-1){
    perror("failed open pipe");
    close(*to_server);
    remove(clientPipe);
    exit(1);
  }

  printf("[client] waiting for server...\n");
  if(read(from_server, &random, sizeof(random)) <= 0){
    perror("fail to read synack");
    close(*to_server);
    close(from_server);
    remove(clientPipe);
    exit(1);
  }

  printf("[client] received SYN_ACK: %d\n", random);
  ack = random + 1;
  printf("[client] sending ack: %d\n", ack);
  if(write(*to_server, &ack, sizeof(int))== -1){
    perror("failed send ack");
    close(*to_server);
    close(from_server);
    remove(clientPipe);
    exit(1);
  }

  printf("[client] handshake complete\n");
  close(from_server);
  remove(clientPipe);

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
  char message[BUFFER_SIZE];
 
  int bytesRead = read(from_client, message, BUFFER_SIZE - 1);
    if(bytesRead <= 0){
      perror("failed read client");
      close(from_client);
      return -1;
    }

  message[bytesRead] = '\0'; 
  printf("[server_connect] Received from client: %s\n", message);
  strcpy(message, "hello");
  int bytesWrite = write(from_client, message, strlen(message) + 1);
  if(bytesWrite <= 0){
    perror("failed write client");
    close(from_client);
    return -1;
  }
  printf("[server_connect] Sent to client: %s\n", message);

  return to_client;
}

void cleanWKP(){
  printf("[server] Cleaning up WKP...\n");
  remove(WKP);
  exit(0);
}