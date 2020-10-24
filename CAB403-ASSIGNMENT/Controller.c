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
#define MIN_REQUIRED 4

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
    int arg_count = argc;

    if (argc < MIN_REQUIRED)
    {
        return 1;
    }
    else
    {
        isValidated = 0;
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
            arg_count += -2;
            printf("out file decteded %s\n", argv[++i]);
            continue;
        }

        if (strcmp("-log", argv[i]) == 0)
        {
            logIndex = i;
            isValidated = 0;
            arg_count += -2;
            printf("log file decteded %s\n", argv[++i]);
            continue;
        }

        //TODO:time
        // if (strcmp("-t", argv[i]) == 0)
        // {
        //     timeIndex = i;
        //     isValidated = 0;
        //     printf("time decteded %s\n", argv[++i]);
        //     continue;
        // }
    }

    //check the order of -o and -log
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

    //check file path was included
    if (arg_count == MIN_REQUIRED)
    {
        isValidated = 0;
    }
    else
    {
        isValidated = 1;
        printf("required file\n");
    }

    return isValidated;
}

void send_data(int argc, char *argv[], int sockfd)
{
    char buffer[SIZEOF_ARG_DATA];
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
}

int main(int argc, char *argv[])
{
    int sockfd, numbytes, result;
    struct hostent *he;
    struct sockaddr_in their_addr; // Clients connector's address information

    //validate the argumenents if no error return 0 otherwise 1
    if(validate_arguments(argc, argv) == 0)
    {
        int port = atoi(argv[2]);

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

        if (connect(sockfd, (struct sockaddr *)&their_addr, sizeof(struct sockaddr)) == -1)
        {
            printf("\n");
            fprintf(stderr, "could not connect to overseer at %s %s\n", argv[1], argv[2]);
            //perror("Could not connect at ");
            exit(1);
        }

        send_data(argc, argv, sockfd);

        /* close the socket */
        close(sockfd);
    }
    else
    {
        /* print the usage message to stderr when arguments are invalid */
        help();
    }
    return 0;
}