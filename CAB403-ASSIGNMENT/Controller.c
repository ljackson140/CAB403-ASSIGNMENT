/*================================================================
    Controller:
    Laku Jackson
    Aung 
    Saw
Acknowledgement of Assignment based off Tutorial 7 
=================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdbool.h>

#define MAXDATASIZE 100 /* max number of bytes we can get at once */
#define MIN_ARGC 3      /* minimum number of arguments */

int main(int argc, char *argv[])
{
    int sockfd, numbytes, result;
    char buf[MAXDATASIZE];
    struct hostent *he;
    struct sockaddr_in their_addr; // Clients connector's address information 

    char *usage = "Usage: controller <address> <port> {[-o out_file] [-log log_file] [-t seconds] <file> [arg...] | mem [pid] | memkill <percent>}";
    
    int port;

    /* if the first argument is --help, output the following */
    result = strcmp(argv[1], "--help");

    if (result == 0 && argc < 3)
    {
        return help(usage);
    }
    else
    {
        /* order the arguments */
        port = atoi(argv[2]);
    }

    if ((he = gethostbyname(argv[1])) == NULL)
    { /* get the host info */
        herror("gethostbyname");
        exit(1);
    }

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket");
        exit(1);
    }

    /* clear address struct */
    memset(&their_addr, 0, sizeof(their_addr));

    their_addr.sin_family = AF_INET;   /* host byte order */
    their_addr.sin_port = htons(port); /* short, network byte order */
    their_addr.sin_addr = *((struct in_addr *)he->h_addr);

    if (connect(sockfd, (struct sockaddr *)&their_addr,
                sizeof(struct sockaddr)) == -1)
    {
        printf("\n");
        fprintf(stderr,"Could not connect at %s %d\n", argv[1], port);
        //perror("Could not connect at ");
        exit(1);
    }

    if ((numbytes = recv(sockfd, buf, MAXDATASIZE, 0)) == -1)
    {
        perror("recv");
        exit(1);
    }

    buf[numbytes] = '\0';

    printf("Received: %s", buf);

    close(sockfd);

    return 0;
}

int help(char *str)
{
    fprintf(stdout, "%s", str);
    return 1;
}

int invalid(char *str)
{
    fprintf(stderr, "%s", str);
    return 1;
}
