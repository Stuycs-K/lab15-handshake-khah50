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

int main() {
  signal(SIGINT, cleanWKP);
  srand(time(NULL));
  int to_client;
  int from_client;
  from_client = server_handshake( &to_client );

  while(1){
    int random = rand() % 101;
    printf("[server] Sending: %d\n", random);
    write(to_client, &random, sizeof(random));
    sleep(1);
  }
  close(to_client);
  close(from_client);
  return 0;
}
