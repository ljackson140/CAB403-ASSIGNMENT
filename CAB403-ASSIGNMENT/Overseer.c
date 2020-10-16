/*================================================================
    Overseer:
    Laku Jackson
    Aung 
    Saw
Acknowledgement of Assignment based off Tutorial 7 
=================================================================*/

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>

/* Amount of penidng connections */
#define BACKLOG 10 

/* Global variables for connectivity */
int sockfd, new_fd;            /* listen on sock_fd, new connection on new_fd */
struct sockaddr_in my_addr;    /* my address information */
struct sockaddr_in their_addr; /* connector's address information */
socklen_t sin_size;

int main(int argc, char *argv[])
{
    /* time variables  */
    struct tm *local;
    time_t overseerTime;
    overseerTime = time(NULL);
    local = localtime(&overseerTime);

    

    /* generate the socket */
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket");
        exit(1);
    }

    if (argc != 2)
    {
        fprintf(stderr, "usage: <Port Number>\n");
        exit(1);
    }

    int myport = atoi(argv[1]);    

    /* Enable address/port reuse, useful for server development */
    int opt_enable = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt_enable, sizeof(opt_enable));
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &opt_enable, sizeof(opt_enable));

    

    
    memset(&my_addr, 0, sizeof(my_addr));

    /* generate the end point */
    my_addr.sin_family = AF_INET;         /* host byte order */
    my_addr.sin_port = htons(myport);     /* short, network byte order */
    my_addr.sin_addr.s_addr = INADDR_ANY; /* auto-fill with my IP */

    if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1) //Bind socket to end point
    {
        perror("bind");
        exit(1);
    }

    if (listen(sockfd, BACKLOG) == -1) //listen for potential client connectivity
    {
        perror("listen");
        exit(1);
    }


    printf("server starts listening ...\n");
 


/*===================================================================================*
Main function:               Main loop of the server
=====================================================================================*/
  

    
    while (1)
    {
        sin_size = sizeof(struct sockaddr_in);
        if ((new_fd = accept(sockfd, (struct sockaddr *)&their_addr,
                             &sin_size)) == -1)
        {
            perror("accept");
            continue;
        }
        //printf("server: got connection from %s\n",
           //    inet_ntoa(their_addr.sin_addr));
        //printf("");

        printf("%d-%d-%d %d:%d:%d", local->tm_year + 1900,local->tm_mon + 1, local->tm_mday, //date
                        local->tm_hour, local->tm_min, local->tm_sec);

        printf(" - connection recieved from %s\n",
                    inet_ntoa(their_addr.sin_addr)); //IP address

        if (!fork())
        { 
            if (send(new_fd, "Hello, world!\n", 14, 0) == -1) //Child process
                perror("send");
            close(new_fd);
            exit(0);
        }
        close(new_fd); 

        while (waitpid(-1, NULL, WNOHANG) > 0)
            ;
    }
}