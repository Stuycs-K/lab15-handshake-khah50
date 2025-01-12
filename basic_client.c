#include "pipe_networking.h"
#include <signal.h>
#include <errno.h>

void cleanup_client(int signum) {
    printf("[client] Exiting...\n");
    exit(0);
}

int main() {
  signal(SIGINT, cleanup_client);
  int to_server;
  int from_server;
  from_server = client_handshake( &to_server );

  while (1){
    int random;
    if(read(from_server, &random, sizeof(random)) <= 0){
      printf("disconected\n");
      break;
    }
    printf("[client] Received: %d\n", random);
  }

  return 0;
}
