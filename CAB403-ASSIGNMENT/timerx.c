#include <time.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{

    /*struct tm {
    int tm_sec; //sec
    int tm_min; // min
    int tm_hour; // hour
    int tm_mday; //day of month
    int tm_month; // month
    int tm_year; // year
    };*/

    time_t curtime;
    time(&curtime);

    
    printf("Cur time = %s", ctime(&curtime));
    return 0;
}
 