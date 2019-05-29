/*****************************************************************************
 * server-c.c
 * Name:
 * NetId:
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <errno.h>

#define QUEUE_LENGTH 10
#define RECV_BUFFER_SIZE 2048

/* TODO: server()
 * Open socket and wait for client to connect
 * Print received message to stdout
 * Return 0 on success, non-zero on failure
*/
// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int server(char *server_port) {
    int sockfd, new_fd;
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr;
    socklen_t sin_size;
    int yes = 1;
    char buf[RECV_BUFFER_SIZE] = {'\0'};
    int rv;
    char s[INET6_ADDRSTRLEN];
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if ((rv = getaddrinfo(NULL, server_port, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }
   for(p = servinfo; p != NULL; p = p->ai_next ) {
  
  	    if((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
	perror("server:socket"); continue;
 }

    if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
	perror("setsockopt"); continue;

    }
    if(bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
	    close(sockfd); perror("server:bind");
        continue;
    }	
    break;


   }
   freeaddrinfo(servinfo);
   if(p == NULL) {
     fprintf(stderr, "server: failed to bind\n");
     exit(1);
   }	
   if(listen(sockfd, QUEUE_LENGTH) == -1) {
       perror("listen");
       exit(1);
   }
    while(1) {
        sin_size = sizeof(their_addr);
        new_fd = accept(sockfd, (struct sockaddr *) & their_addr, &sin_size);
        if(new_fd == -1) {
            perror("accept");
            continue;
        }
        inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr * )&their_addr), s, sizeof(s));
        int numbytes;
       while((numbytes = recv(new_fd, buf, RECV_BUFFER_SIZE - 1, 0)) > 0){
            write(1, buf, numbytes);
            fflush(stdout);

       }
       close(new_fd);
    }



    return 0;
}

/*
 * main():
 * Parse command-line arguments and call server function
*/
int main(int argc, char **argv) {
  char *server_port;

  if (argc != 2) {
    fprintf(stderr, "Usage: ./server-c [server port]\n");
    exit(EXIT_FAILURE);
  }

  server_port = argv[1];
  return server(server_port);
}
