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
#define MIN_REQUIRED 3

int validate_arguments(int argc, char *argv[]);
// int usage(char *str);
// int invalid(char *str);
int help()
{
    fprintf(stdout, "Usage: <address> <port> {[-o out_file] [-log log_file] [-t seconds]<file> [arg...] | mem [pid] | memkill <percent>}\n");
    return 1;
}

int validate_arguments(int argc, char *argv[])
{
    int isValidated = 1;
    int outIndex = 0;
    int logIndex = 0;

    if (argc < MIN_REQUIRED)
    {
        return 1;
    }

    //iterate the arguments
    for (int i = 1; i < (argc - 1); i++)
    {
        if (strcmp("--help", argv[i]) == 0)
        {
            isValidated = 1;
            return help();
            continue;
        }
        
        if (strcmp("-o", argv[i]) == 0)
        {
            outIndex = i;
            isValidated = 0;
            printf("out file decteded %s\n", argv[++i]);
            continue;
        }

        if (strcmp("-log", argv[i]) == 0)
        {
            logIndex = i;
            isValidated = 0;
            printf("log file decteded %s\n", argv[++i]);
            continue;
        }

        if (strcmp("-log", argv[i]) == 0)
        {
            logIndex = i;
            isValidated = 0;
            printf("log file decteded %s\n", argv[++i]);
            continue;
        }
    }

    if (logIndex > 0)
    {
        if (outIndex < logIndex)
        {
            isValidated = 0;
            printf("order correct\n");
        }
        else
        {
            isValidated = 1;
            printf("order wrong\n");
        }
    }
    return isValidated;
}

int main(int argc, char *argv[])
{
    char buffer[SIZEOF_ARG_DATA];
    int sockfd, numbytes, result;
    struct hostent *he;
    struct sockaddr_in their_addr; // Clients connector's address information

    char *usageMessage = "Usage: <address> <port> {[-o out_file] [-log log_file] [-t seconds]<file> [arg...] | mem [pid] | memkill <percent>}\n";

    //validate the argumenents if no error return 0 otherwise 1
    if(validate_arguments(argc, argv) == 0)
    {
        int port = atoi(argv[2]);
        // if (argc != 3)
        // {
        //     fprintf(stderr, "usage: <address> <port>\n");
        //     exit(1);
        // }

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
            fprintf(stderr, "could not connect to overseer at %s %d\n", argv[1], port);
            //perror("Could not connect at ");
            exit(1);
        }

        //TODO: Get contents from command line to send to server
        char *send_file;
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
            for (int i = 0; i < strlen(argv[num_args]); i++)
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
    }
    else
    {
        //do something
        help();
    }
    
    
    return 0;
}

// int usage(char *str)
// {
//     fprintf(stdout, "%s", str);
//     return 1;
// }

// int invalid(char *str)
// {
//     fprintf(stderr, "%s", str);
//     return 1;
// }