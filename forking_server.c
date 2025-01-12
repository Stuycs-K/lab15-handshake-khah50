#include "pipe_networking.h"
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>

void cleanWKP(){
  printf("[server] Cleaning up WKP...\n");
  remove(WKP);
  exit(0);
}

void handle_client(int from_client){
    int to_client;
    server_handshake_half(&to_client, from_client);
    
    srand(getpid());
    while(1){
        int random = rand() % 101;
        printf("[subserver %d] Sending: %d\n", getpid(), random);
        if(write(to_client, &random, sizeof(random)) <= 0){
            printf("disconnected\n");
            break;
        }
        sleep(1);
    }

    close(to_client);
    close(from_client);
    exit(0);
}

int main() {
  signal(SIGINT, cleanWKP);

  while(1){
    int from_client = server_setup();
    if(fork() == 0){
        handle_client(from_client);
    }
    close(from_client);
  }

  return 0;
}
