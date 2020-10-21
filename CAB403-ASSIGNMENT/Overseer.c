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

/* Amount of pending connections */
#define BACKLOG 10 
#define SIZEOF_ARG_DATA 100

/* Global variables for connectivity */
int sockfd, new_fd, retrieve, num_bytes;            /* listen on sock_fd, new connection on new_fd */
char buffer[SIZEOF_ARG_DATA];
struct sockaddr_in my_addr;    /* my address information */
struct sockaddr_in their_addr; /* connector's address information */
socklen_t sin_size;


/*=========================

    Executing file function

===========================*/

int file_exec(char * filepath, char * filename[])
{
    retrieve = execv(filepath, filename);
    retrieve = 20;
    perror("unable to execute");
    return retrieve;
}

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

        printf("%d-%d-%d %d:%d:%d - connection recieved from %s\n", local->tm_year + 1900,local->tm_mon + 1, local->tm_mday, //date
                        local->tm_hour, local->tm_min, local->tm_sec, inet_ntoa(their_addr.sin_addr));

        /*printf(" - connection recieved from %s\n",
                    inet_ntoa(their_addr.sin_addr)); //IP address*/

        /*if (!fork())
        { 
            if (send(new_fd, "Hello, world!\n", 14, 0) == -1) //Child process
                perror("send");
            close(new_fd);
            exit(0);
        }
        close(new_fd);*/

        //TODO: Recieve args from controller
        if ((num_bytes = recv(new_fd, buffer, SIZEOF_ARG_DATA, 0)) == -1)
        {
            perror("recv");
            exit(1);
        }

        //TODO: logging first execution
        fprintf(stdout,"%d-%d-%d %d:%d:%d - attempting to execute %s\n", local->tm_year + 1900,local->tm_mon + 1, local->tm_mday, //date
                        local->tm_hour, local->tm_min, local->tm_sec, buffer); 

        //TODO: Split up buffer to retrieve arguments and filepath
        char * filepath = buffer;
        int ptr_tokens = 0;
        char * storage_toks[25];
        storage_toks[0] = strtok(buffer, " "); //fetch first token within string upon space

        while (storage_toks[ptr_tokens] != NULL)
        {
            ptr_tokens++;
            storage_toks[ptr_tokens] = strtok(NULL, " ");
        }

        char * filename[ptr_tokens+1];
        memset(filename, 0, ptr_tokens + 1); //pointer to memory

        //TODO: store agrs into an array
        for (int i = 0; i < ptr_tokens; i++)
        {
            filename[i] = storage_toks[i];
        }
        filename[ptr_tokens] = (char *) NULL; /*last elemnet is NULL*/

        //TODO: fork file
        pid_t file_execution = fork();
        int update;
        int status_code = 0;

        if(file_execution == -1)
        {
            fprintf(stdout,"%d-%d-%d %d:%d:%d - could not execute %s\n", local->tm_year + 1900,local->tm_mon + 1, local->tm_mday, //date
                        local->tm_hour, local->tm_min, local->tm_sec, buffer);
        } else if (file_execution == 0)
        {
            status_code = file_exec(filename[0], filename);
            fprintf(stdout,"%d-%d-%d %d:%d:%d - %s has been executed with pid %d\n", local->tm_year + 1900,local->tm_mon + 1, local->tm_mday, //date
                        local->tm_hour, local->tm_min, local->tm_sec, buffer, getpid());
        } else
        {
            wait(&update);
            fprintf(stdout,"%d-%d-%d %d:%d:%d - %d has terminated with status code %d\n", local->tm_year + 1900,local->tm_mon + 1, local->tm_mday, //date
                        local->tm_hour, local->tm_min, local->tm_sec, file_execution, status_code);
        }
        
        
        


        
        

        while (waitpid(-1, NULL, WNOHANG) > 0)
            ;
    }
}

//Testing
