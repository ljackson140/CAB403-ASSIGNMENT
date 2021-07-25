/*===============================
    func: prints current time 
===============================*/
void Curr_Time()
{
    time_t ShowTime = time(NULL);
    struct tm local = *localtime(&ShowTime);
    fprintf(stdout, "%d-%d-%d %d:%.2d:%.2d - ", local.tm_year + 1900,local.tm_mon + 1, local.tm_mday, local.tm_hour, local.tm_min, local.tm_sec);
}