#include <stdio.h>
#include <unistd.h>

int main()
{
    printf("This message is sent to stdout. In 3 seconds i will terminate with a status code 5\n");
    sleep(5);
    return 5;
}
