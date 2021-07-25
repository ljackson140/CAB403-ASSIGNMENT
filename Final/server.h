#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <netdb.h>
#include <regex.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <arpa/inet.h>

#define MAXIMUM_THREADS 5
#define ARG_BUF_SIZE 2000
#define _MAX_CMDS 30

/*=========================
    Struct task
===========================*/
typedef struct task
{
    void*(*main_routine)(void *);     
    struct task *next;
    void *args;
} tpool_node;



/*=========================
    Struct for threadpool
===========================*/
typedef struct Thread_pl
{
    size_t cl_pool;           
    size_t M_NUM_THREAD;       
    pthread_t *ID_thread;
    pthread_cond_t Qready; 
    pthread_mutex_t lockQ;       
    tpool_node *Thread_Pool_Hed;     
     
} tpool_t;

//recv a Client_String from the controller, will be put in the main loop
/*=========================
    func: recieves controller 
    args and store them into 
    the arg buffer
===========================*/
char *ControllerString();


/*=========================
    func: Execute program with given filename 
    and any other passed commandline Args 
===========================*/
void ExecProgram();


/*=========================
    func: searches for filename within the commandline args
    then returns an integer of the files location
===========================*/
int checkFile();


/*=========================
    func: redirect output to a file
===========================*/
int Dup_Acc();

//reverses the redirection to a file
/*=========================
    func:reverses 
===========================*/
void Dup_Cl();


/*=========================
    func: Handles the past arguments sennt from controller 
    then creates out files if log is found
===========================*/
void *argument_listener();