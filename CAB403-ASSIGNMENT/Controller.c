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

#define SIZEOF_ARG_DATA 100 /* max number of bytes we can get at once */

int main(int argc, char *argv[])
{
<<<<<<< HEAD
    char buffer[SIZEOF_ARG_DATA];
    int sockfd, numbytes;
=======
    int sockfd, numbytes, result;
    char buf[MAXDATASIZE];
>>>>>>> 4d5fecd62194782d9745abc6cc5c091ea8f29133
    struct hostent *he;
    struct sockaddr_in their_addr; // Clients connector's address information 

    if (argc != 3)
    {
        fprintf(stderr, "usage: <address> <port>\n");
        exit(1);
    }

<<<<<<< HEAD
    if (argc < 3)
    {
        fprintf(stderr, "usage: <address> <port> {[-o out_file] [-log log_file] [-t seconds]<file> [arg...] | mem [pid] | memkill <percent>}\n");
        exit(1);
    }

    

    int port = atoi(argv[2]);  
    
        
=======
    /* if the first argument is --help, output the following */
    result = strcmp(argv[1], "--help");

    if (result == 0)
    {
        printf("Usage: controller <address> <port> {[-o out_file] [-log log_file] [-t seconds] <file> [arg...] | mem [pid] | memkill <percent>}");
    }
    else
    {
        /* order the arguments */
        
    }
    

    int port = atoi(argv[2]);
>>>>>>> 4d5fecd62194782d9745abc6cc5c091ea8f29133

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
        fprintf(stderr,"could not connect to overseer at %s %d\n", argv[1], port);
        //perror("Could not connect at ");
        exit(1);
    }


    //TODO: Get contents from command line to send to server
    char* send_file;
    if (argc >= 3)
    {
        send_file = argv[3];
    }

    //TODO: Use buffer to store filepath
    int file_buffer = 0;
    printf("%d\n", strlen(send_file));
    for (int i = 0; i < strlen(send_file); i++)
    {
        buffer[i] = send_file[i];
        file_buffer++;
    }

    //TODO: Use buffer to store args
    int num_args = 4;
    while (num_args < argc)
    {
        buffer[file_buffer++] = ' ';
        for(int i = 0; i < strlen(argv[num_args]); i++)
        {
            buffer[file_buffer++] = argv[num_args][i];
        }
        num_args++;
    }

    //test1
    printf("%d\n", file_buffer);

    buffer[file_buffer++] = '\0';
    printf("%s", buffer);

    if (send(sockfd, buffer, file_buffer, 0) == -1)
    {
        perror("Arguments have been sent");
    }

    close(sockfd);

    return 0;
}