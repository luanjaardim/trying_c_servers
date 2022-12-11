#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> //defines the hostent struct
#define pt pthread_
#define f(a, b) for(int a = 0; a < b; a++)

//error handling
#define handle_error(msg) \
           do { perror(msg); exit(EXIT_FAILURE); } while (0)

int main(int argc, const char **argv) {

  if(argc < 3) handle_error("you must provide a server address and a port number");

  struct hostent *server = gethostbyname(argv[1]);
    if(server == NULL) handle_error("invalid address for server");

  uint16_t port_num = atoi(argv[2]);

  int ret;
  int sockfd; //socket file descriptor
  struct sockaddr_in server_addr;

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0) handle_error("error creating client socket");

  memset((void *) &server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  memcpy((void *) &server_addr.sin_addr.s_addr, (void *) server->h_addr_list[0], server->h_length);
  //copying the address from command line to our server_addr
  server_addr.sin_port = htons(port_num);

  ret = connect(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr));
    if(ret < 0) handle_error("fail to connect");

  char buffer[256];
  while(1){
    memset((void *) buffer, '\0', 256);
    fgets(buffer, 255, stdin);
    // scanf(" %255[^\n]", buffer); //typing a message to server

    ret = write(sockfd, (void *) buffer, strlen(buffer));
      if(ret < 0) handle_error("writing error");

    memset((void *) buffer, '\0', 256);
    ret = read(sockfd, (void *) buffer, 255);
      if(ret < 0) handle_error("reading error");

    printf("Host: %s\n", buffer);

    if(!strncmp("Bye", buffer, 3) || !strcmp("", buffer)) break; //loop exit condition
  }
  
  //closing sockets
  close(sockfd);

  return 0;
}