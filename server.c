#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/file.h> //constains definitions of a lot of datas types used in system calls
#include <sys/socket.h> //for working with sockets
#include <netinet/in.h> //constains constants and structures for net domain addres
#define pt pthread_
#define f(a, b) for(int a = 0; a < b; a++)

//error handling
#define handle_error(msg) \
           do { perror(msg); exit(EXIT_FAILURE); } while (0)

int main(int argc, const char **argv) {

  if(argc < 2) handle_error("you must provide a port number");
  uint16_t port_num = atoi(argv[1]); //uint16_t is the same of unsigned short

  int ret; //for handle errors
  int server_sock; //descriptor for server socket, that's the communication endpoint for talk with the server 
  struct sockaddr_in server_addr, client_addr;

  server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if(server_sock < 0) handle_error("error creating server socket");

  memset((void *) &server_addr, 0, sizeof(server_addr)); //cleaning server_addr

  //configuration of server_addr
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(port_num); //htons convert the port number to network byte order
  //end of configuration

  //binding server_sock to our addr
  ret = bind(server_sock, (struct sockaddr *) &server_addr, sizeof(server_addr));
    if(ret < 0) handle_error("binding error");

  ret = listen(server_sock, 2); //starting to listen
    if(ret < 0) handle_error("listening error");

  socklen_t size = sizeof(client_addr);
  int new_connection_sock = accept(server_sock, (struct sockaddr *) &client_addr, &size); //accepting connection with client
    if(new_connection_sock < 0) handle_error("fail to accept");
  /* 
    a new socket was generated because our server needs a communication endpoint(socket) for 
    receive another communication requests(server_sock) and a new one for communicate with
    the specific client that was accepted 
  */

  char buffer[256];
  while(1){
    memset((void *) buffer, '\0', 256); //cleaning buffer
    ret = read(new_connection_sock, (void *) buffer, 255); //waiting messages from client
      if(ret < 0) handle_error("reading error");

    printf("Client: %s\n", buffer);

    memset((void *) buffer, '\0', 256); //cleaning buffer
    // scanf("%255[^\n]", buffer); //typing a message back to client
    fgets(buffer, 255, stdin);

    ret = write(new_connection_sock, (void *) buffer, strlen(buffer)); //sending back a message
      if(ret < 0) handle_error("writing error");

    if(!strncmp("Bye", buffer, 3)) break; //loop exit condition
  }

  //closing sockets
  close(server_sock);
  close(new_connection_sock);

  return 0;
}