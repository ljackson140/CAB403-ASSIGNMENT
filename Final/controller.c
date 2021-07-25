#include "server.h"

#define MIN_REQUIRED 4 /*minimum requried arguments*/
#define ARGUMENT_COUNT 4 /*possible command arguments*/

/* show usage message to stdout */
int show_usage()
{
    fprintf(stdout, "Usage: controller <address> <port> {[-o out_file] [-log log_file] [-t seconds] <file> [arg...] | mem [pid] | memkill <percent>}\n");
    exit(1);
    return 1;
}

/* show usage message to stderr */
int show_error()
{
    fprintf(stderr, "Usage: controller <address> <port> {[-o out_file] [-log log_file] [-t seconds] <file> [arg...] | mem [pid] | memkill <percent>}\n");
    //exit(1);
    return 1;
}

/* validate the arguments according to usage syntax */
bool validate_arguments(int argc, char *argv[])
{
    bool result = false;
    int commands[ARGUMENT_COUNT] = {0,0,0,0};
    int arg_count = argc;

    //check the minimum argument
    if (argc < MIN_REQUIRED)
    {
        return false;
    }
    else
    {
        result = true;
    }
    
    //check the arguments using iteration
    for (int i = 1; i < argc; i++)
    {
        if (strcmp("--help", argv[i]) == 0)
        {
            show_usage();
        }
        
        if (strcmp("-o", argv[i]) == 0)
        {
            commands[0] = i;
            result = true;
            arg_count += -2;
            continue;
        }

        if (strcmp("-log", argv[i]) == 0)
        {
            commands[1] = i;
            result = true;
            arg_count += -2;
            continue;
        }

        if (strcmp("-t", argv[i]) == 0)
        {
            commands[2] = i;
            result = true;
            arg_count += -2;
            continue;
        }

        if (strcmp("mem", argv[i]) == 0 || strcmp("memkill", argv[i]) == 0)
        {
            commands[3] = i;
            result = true;
            arg_count += -2;

            //mem should be last argument
            if (i == (argc-2))
            {
                result = true;
            }
            else
            {
                return false;
            }
            
            continue;
        }
    }

    int temp = 0;
    //check the order of arguments, -o -log -t mem
    for (int i = 0; i < ARGUMENT_COUNT; i++)
    {
        if (commands[i] != 0)
        {
            if (commands[i] < temp)
            {
                return false;
            }
            else
            {
                result = true;
            }
            temp = commands[i];
        }
        else
        {
            result = true;
        }
    }

    //check the argument count
    if (arg_count >= MIN_REQUIRED)
    {
        if (commands[3] == 0)
        {
            if (strspn(argv[argc-2], "-") > 0)
            {
                result = false;
            }
            else
            {
                result = true;
            }
        }
        else
        {
            result = true;
        }
    }
    else
    {
        result = false;
    }
    //return the value of validation
    return result;
}

/* generate a socket on and return the socket number to be used */
int getSocketID()
{
    int socketID;
    if((socketID = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket");
        exit(1);
    }
    return socketID;
}

/* gets the port number from the commandline args */
int getPortNumber(char *arguments[])
{
    //check the port number is digit
    if (strspn(arguments[2], "0123456789") != strlen(arguments[2]))
    {
        show_usage();
        exit(1);
    }

    int portNumber;
    portNumber = atoi(arguments[2]);

    //check host
    if (gethostbyname(arguments[1]) == NULL)
    {
        fprintf(stderr, "Could not connect to overseer at %s %s\n", arguments[1], arguments[2]);
        exit(1);
    }

    return portNumber;
}

//connects their address struct onto the socket number
void init_socket(int socketID, int portNum)
{
    //set up their socket in the sock struct
    struct sockaddr_in theirAddress;

    //clear the struct
    memset(&theirAddress, 0, sizeof(theirAddress));
    //gen end point
    theirAddress.sin_family = AF_INET;
    theirAddress.sin_port = htons(portNum);
    theirAddress.sin_addr.s_addr = INADDR_ANY;

    if (connect(socketID, (struct sockaddr *)&theirAddress, sizeof(struct sockaddr)) == -1)
    {
        printf("could not connect to overseer at <%s> <%d>\n", inet_ntoa(theirAddress.sin_addr), portNum);//print 
        exit(1);
    }
}

//send a string over a socket
void sendString(int socketID, char string[])
{
    if(send(socketID, string, strlen(string), 0) == -1)
    {
        perror("send");
        exit(0);
        close(socketID);
    }
}

//send however many args over to the overseer with a " " in between
void sendArguments(int socketID, int argc, char *argv[])
{
    for (int i = 0; i < argc; i++)
    {
        sendString(socketID, argv[i]);
        sendString(socketID, " ");
    }
}

int main(int argc, char *argv[])
{
    //use validation
    if (!validate_arguments(argc, argv))
    {
        show_error();
        exit(1);
    }

    //socketID
    int socketID = getSocketID();
    //valid port to connect
    int port = getPortNumber(argv);

    //connect to socket
    init_socket(socketID, port);

    //send argurments to overseer
    sendArguments(socketID, argc, argv);

    close(socketID);
    
}
