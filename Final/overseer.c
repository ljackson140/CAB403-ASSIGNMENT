/*================================================================
    Overseer:
    Laku Jackson
    Aung 
    Saw
Acknowledgement of Assignment based off Tutorial 7 
=================================================================*/

/* includes the header file here */
#include "server.h"
#include "overseer_helper.h"

tpool_t *MAIN_THREAD = NULL;
static void *main_routine(void *args)
{
    tpool_t *Main_Pool = (tpool_t *)args;
    tpool_node *task = NULL;

    while (1)
    {
        pthread_mutex_lock(&Main_Pool->lockQ);
        while (!Main_Pool->Thread_Pool_Hed && !Main_Pool->cl_pool)
        {
            pthread_cond_wait(&Main_Pool->Qready, &Main_Pool->lockQ);
        }

        if (Main_Pool->cl_pool)
        {
            pthread_mutex_unlock(&Main_Pool->lockQ);
            pthread_exit(NULL);
        }

        task = Main_Pool->Thread_Pool_Hed;
        Main_Pool->Thread_Pool_Hed = (tpool_node *)Main_Pool->Thread_Pool_Hed->next;
        pthread_mutex_unlock(&Main_Pool->lockQ);

        task->main_routine(task->args);

        free(task);
    }
    return NULL;
}

/* start thread */
int init_thread(tpool_t **Main_Pool, size_t ThreadMax)
{
    (*Main_Pool) = (tpool_t *)malloc(sizeof(tpool_t));
    if (*Main_Pool == NULL)
    {
        printf("Malloc failed\n");
        exit(-1);
    }
    (*Main_Pool)->cl_pool = 0;
    (*Main_Pool)->M_NUM_THREAD = ThreadMax;
    (*Main_Pool)->ID_thread = (pthread_t *)malloc(sizeof(pthread_t) * ThreadMax);
    if ((*Main_Pool)->ID_thread == NULL)
    {
        printf("Cannot initialise thread id\n");
        exit(-1);
    }
    (*Main_Pool)->Thread_Pool_Hed = NULL;
    if (pthread_cond_init(&((*Main_Pool)->Qready), NULL) != 0)
    {
        printf("Cannot initialise cond\n");
        exit(-1);
    }
    if (pthread_mutex_init(&((*Main_Pool)->lockQ), NULL) != 0)
    {
        printf("Cannot initialise mutex\n");
        exit(-1);
    }

    int i = 0;
    while (i < ThreadMax)
    {
        if (pthread_create(&((*Main_Pool)->ID_thread[i]), NULL, main_routine, (void *)(*Main_Pool)) != 0)
        {
            printf("pthread failed to be created\n");
            exit(-1);
        }
        i++;
    }
    return 0;
}

int AddtoQ(tpool_t *Main_Pool, void *(*routine)(void *), void *args)
{
    tpool_node *task;
    tpool_node *_mem;

    if (!routine)
    {
        printf("routine is null\n");
        return -1;
    }

    task = (tpool_node *)malloc(sizeof(tpool_node));
    if (!task)
    {
        printf("in %s,malloc task error, errno = %d, explain:%s\n", __func__, errno, strerror(errno));
        return -1;
    }

    task->main_routine = routine;
    task->args = args;
    task->next = NULL;

    pthread_mutex_lock(&Main_Pool->lockQ);
    _mem = Main_Pool->Thread_Pool_Hed;
    if (!_mem)
    {
        Main_Pool->Thread_Pool_Hed = task;
    }
    else
    {
        while (_mem->next)
        {
            _mem = (tpool_node *)_mem->next;
        }

        _mem->next = task;
    }

    pthread_cond_signal(&Main_Pool->Qready);
    pthread_mutex_unlock(&Main_Pool->lockQ);
    return 0;
}

int avertOutPut(int num_arg, char *cmdArgs[])
{
    int _FileOutPUT = -1;
    for (int i = 0; i < num_arg; i++)
    {
        if (strcmp(cmdArgs[i], "-o") == 0)
        {
            _FileOutPUT = open(cmdArgs[i + 1], O_WRONLY | O_CREAT | O_APPEND, 0666);

            dup2(_FileOutPUT, STDOUT_FILENO);
            dup2(_FileOutPUT, STDOUT_FILENO);

            close(_FileOutPUT);
            break;
        }
    }
    return _FileOutPUT;
}


//checks the correct amount of args has been given
int validate_argument(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: <Port Number>\n");
        exit(1);
    }

    //check the port number is digit
    if (strspn(argv[1], "0123456789") != strlen(argv[1]))
    {
        fprintf(stderr, "Usage: <Port Number>\n");
        exit(1);
    }
    return 1;
}

/* get port number from arguments */
int get_port(char *cmdArgs[])
{
    return atoi(cmdArgs[1]);
}

/* generate the socket and return it */
int prod_Soc()
{
    int NumOf_Soc;
    if ((NumOf_Soc = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket");
        exit(1);
    }
    return NumOf_Soc;
}

/* to reuse port */
void usePortAgain(int NumOf_Soc)
{
    int opt_enable = 1;
    setsockopt(NumOf_Soc, SOL_SOCKET, SO_REUSEADDR, &opt_enable, sizeof(opt_enable));
    setsockopt(NumOf_Soc, SOL_SOCKET, SO_REUSEPORT, &opt_enable, sizeof(opt_enable));
}

/* check -log argument and return the file location */
int checkLog(int lenOf_Args, char *cmdArgs[])
{
    int Locate_log = 0;

    int i = 0;
    while (i < lenOf_Args)
    {
        if (strcmp(cmdArgs[i], "-log") == 0)
        {
            Locate_log = i + 1;
        }
        i++;
    }
    return Locate_log;
}

/* listen incoming connection and add  */
int handle_incoming_connection(int NumOf_Soc)
{
    struct sockaddr_in theirAddress;
    socklen_t sin_size;
    int client_connection;

    while (1)
    {
        sin_size = sizeof(struct sockaddr_in);
        if ((client_connection = accept(NumOf_Soc, (struct sockaddr *)&theirAddress, &sin_size)) == -1)
        {
            perror("accept");
            continue;
        }
        printf("\n");
        Curr_Time();
        fprintf(stdout, "connection received from %s\n", inet_ntoa(theirAddress.sin_addr));

        int *nc = &client_connection;
        AddtoQ(MAIN_THREAD, argument_listener, nc);
    }

    return client_connection;
}

void run_program(int lenOf_Args, char *cmdArgs[], int file_index, int log_index, int file_log, int redeemedSTD_outPut)
{
    pid_t ExecFork = fork(); // fork the child
    int status_code;

    if (ExecFork == 0)
    {
        ExecProgram(lenOf_Args, file_index, cmdArgs);

        if (log_index > 0) // if log then redirect ouput to log file else print to terminal
        {
            redeemedSTD_outPut = Dup_Acc(file_log);
        }
        Curr_Time();
        printf("could not execute %s ", cmdArgs[file_index]);

        for (int i = file_index + 1; i < lenOf_Args; i++)
        {
            printf("%s ", cmdArgs[i]);
        }
        printf("\n");
        //closes the file_log
        if (log_index > 0)
        {
            Dup_Cl(redeemedSTD_outPut);
        }
    }
    else // parent processs
    {
        wait(&status_code); // wait on the child to end

        if (log_index > 0)
        {
            redeemedSTD_outPut = Dup_Acc(file_log);
        }

        Curr_Time();
        if (cmdArgs[file_index + 1] == NULL)
        {
            fprintf(stdout, "%s has been executed with pid %d\n", cmdArgs[file_index], ExecFork);
        }
        else
        {
            fprintf(stdout, "%s %s has been executed with pid %d\n", cmdArgs[file_index], cmdArgs[file_index + 1], ExecFork);   
        }

        Curr_Time();
        fprintf(stdout, "%d has terminated with status code %d\n", ExecFork, WEXITSTATUS(status_code));

        /* closes the log file */
        if (log_index > 0)
        {
            Dup_Cl(redeemedSTD_outPut);
            close(file_log);
        }
    }
}

void programHandler(int lenOf_Args, char *cmdArgs[])
{
    int file_index = checkFile(lenOf_Args, cmdArgs); // file placement in arguments
    int log_index = checkLog(lenOf_Args, cmdArgs); // confirm log in arguments

    bool isLog = false;
    int file_log = 0;
    int redeemedSTD_outPut = -1; // file description stdout

    if (log_index > 0)
    {
        file_log = open(cmdArgs[log_index], O_WRONLY | O_CREAT | O_APPEND, 0666);
        redeemedSTD_outPut = Dup_Acc(file_log);
    }

    Curr_Time();
    fprintf(stdout, "attempting to execute %s", cmdArgs[file_index]);

    int mem_index = file_index + 1;
    if (mem_index < lenOf_Args)
    {
        if (strcmp(cmdArgs[mem_index], "mem") != 0 || strcmp(cmdArgs[mem_index], "memkill") != 0)
        {
            printf(" %s", cmdArgs[mem_index]);
        }
    }
    printf("\n");

    if (log_index > 0)
    {
        Dup_Cl(redeemedSTD_outPut); // close the log file
    }

    run_program(lenOf_Args, cmdArgs, file_index, log_index, file_log, redeemedSTD_outPut);
}

/* tasks to be handled in the pool */
void *argument_listener(int *args)
{
    int Client_Con = *args;

    char *Client_String = ControllerString(Client_Con);

    char *cmdArgs[_MAX_CMDS];

    int lenOf_Args = 0;

    char *str_words = strtok(Client_String, " ");
    while (str_words != NULL)
    {
        int len = strlen(str_words);
        char *sent_string = calloc(len, 10);
        strcat(sent_string, str_words);
        cmdArgs[lenOf_Args] = sent_string;
        str_words = strtok(NULL, " ");
        lenOf_Args++;
    }
    
    programHandler(lenOf_Args, cmdArgs);

    return 0;
}

int Dup_Acc(int F_Desc)
{
    int redeemedSTD_outPut = dup(1);
    dup2(F_Desc, STDOUT_FILENO);

    return redeemedSTD_outPut;
}

void Dup_Cl(int redeemedSTD_outPut)
{
    dup2(redeemedSTD_outPut, STDOUT_FILENO);
}

/* bind the address to the socket */
void init_sockets(int port, int socket_count)
{
    struct sockaddr_in my_Adr; // setup socket in the sock struct
    memset(&my_Adr, 0, sizeof(my_Adr)); // clear the struct

    my_Adr.sin_family = AF_INET;
    my_Adr.sin_port = htons(port);
    my_Adr.sin_addr.s_addr = INADDR_ANY;

    // bind error
    if (bind(socket_count, (struct sockaddr *)&my_Adr, sizeof(struct sockaddr)) == -1)
    {
        perror("bind");
        exit(1);
    }

    // listen error
    if (listen(socket_count, 10) == -1)
    {
        perror("listen");
        exit(1);
    }

    printf("Server is now listening\n");
}

/* receive the controller arguments and store them into buffer */
char *ControllerString(int NumOf_Soc)
{
    char *str_buf = malloc(sizeof(char) * ARG_BUF_SIZE);

    for (int i = 0; i < ARG_BUF_SIZE; i++)
    {
        str_buf[i] = '\0';
    }

    int Byt_Num = 0;

    if ((Byt_Num = recv(NumOf_Soc, str_buf, ARG_BUF_SIZE, MSG_WAITALL) == -1))
    {
        perror("recv");
        exit(1);
    }

    return str_buf;
}

/* checking the filename */
int checkFile(int lenOf_Args, char *cmdArgs[])
{
    int file_index = 3;
    for (int i = 0; i < lenOf_Args; i++)
    {
        if (strcmp(cmdArgs[i], "-o") == 0)
        {
            file_index = file_index + 2;
        }

        if (strcmp(cmdArgs[i], "-log") == 0)
        {
            file_index = file_index + 2;
        }

        if (strcmp(cmdArgs[i], "-t") == 0)
        {
            file_index = file_index + 2;
        }
    }
    return file_index;
}

/* function to execute the program in argument */
void ExecProgram(int lenOf_Args, int fileNumber, char *cmdArgs[])
{
    int _FileOutPUT = avertOutPut(lenOf_Args, cmdArgs);
    int NumFileArgs = lenOf_Args - fileNumber;
    char *ArgsofFile[NumFileArgs];

    for (int y = 0, x = fileNumber; y < NumFileArgs; y++, x++)
    {
        ArgsofFile[y] = cmdArgs[x];
    }

    ArgsofFile[NumFileArgs] = NULL;

    if (execv(ArgsofFile[0], ArgsofFile) == -1)
    {
        if (_FileOutPUT > -1)
        {
            dup2(dup(1), STDOUT_FILENO); // output
            dup2(dup(2), STDOUT_FILENO); // error
            close(_FileOutPUT); // close the file
        }
    }
}

/* main method to run the overseer */
int main(int argc, char *argv[])
{
    validate_argument(argc, argv);

    /* threadpool created */
    if (init_thread(&MAIN_THREAD, MAXIMUM_THREADS) != 0)
    {
        return -1;
    }

    int myPort = get_port(argv); // get the port number from argument
    int socket_count = prod_Soc();
    usePortAgain(socket_count);

    init_sockets(myPort, socket_count);

    int client = handle_incoming_connection(socket_count);

    close(client); // close the connection
}