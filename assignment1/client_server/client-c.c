 /* Name:
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

#define SEND_BUFFER_SIZE 2048


/* TODO: client()
 * Open socket and send message from stdin.
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
int client(char *server_ip, char *server_port) {
  int sockfd, numbytes;
  char buf[SEND_BUFFER_SIZE];
  char s[INET6_ADDRSTRLEN];
  struct addrinfo hints, * servinfo, *p;
  int rv;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  
  if( rv = getaddrinfo(server_ip, server_port, &hints, &servinfo) != 0) {
    	fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
	return 1;
  }  
  for( p = servinfo; p != NULL; p = p->ai_next) {
  	if((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1 )
	{
	    perror("client:socket");
            continue;
	}
    if(connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
	    close(sockfd);
	    perror("client: connect");
        continue;
	}
	break;
  }
  if(p == NULL) {
   fprintf(stderr, "client: failed to connect."); return 2;
  }
  inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),s, sizeof(s));	
  printf("client connecting to %s\n",s);
  freeaddrinfo(servinfo);
  while((numbytes = read(0, buf,  SEND_BUFFER_SIZE - 1)) != 0) {
    if(send(sockfd, buf, numbytes, 0) == -1) 
      perror("client: send");
  }

  close(sockfd);
  return 0;
}

/*
 * main()
 * Parse command-line arguments and call client function
*/
int main(int argc, char **argv) {
  char *server_ip;
  char *server_port;

  if (argc != 3) {
    fprintf(stderr, "Usage: ./client-c [server IP] [server port] < [message]\n");
    exit(EXIT_FAILURE);
  }

  server_ip = argv[1];
  server_port = argv[2];
  return client(server_ip, server_port);
}
