#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <syslog.h>
#include <unistd.h>

#define CHECK "/usr/bin/webservice status"
#define RESULT "Your webservice is running"
#define RECOVERY "/usr/bin/webservice start"

int read_conf()
{
    return 0;
}

int main(int argc, char* argv[])
{
    /* first we read the configuration file*/
    if (read_conf())
        return 1;
    /* Fork off the parent process */
    pid_t pid = fork();
    if (pid < 0)
    {
        perror ("Failed to daemonize itself call to fork() failed");
        return 60;
    }
    if (pid > 0)
        return 0;
    /* this is just a temporary hack in future we will be able to change this using some nice config file */
    size_t check_size = strlen(RESULT);
    FILE *c;
    char result[200];
    int running = 1;
    while (running)
    {
        c = popen(CHECK, "r");
        if (!c)
        {
            syslog(LOG_ALERT, "failed to run /usr/bin/webservice status");
        } else
        {
            if (fgets(result, sizeof(result)-1, c) != NULL)
            {
                if (strlen(result) > check_size)
                {
                    result[check_size] = '\0';
                }
                if (strcmp(result, RESULT) != 0)
                {
                    syslog(LOG_ALERT, "webservice is down, recovering it");
                    pclose(c);
                    c = popen(RECOVERY, "r");
                    if (!c)
                        syslog(LOG_ALERT, "failed to restart the webservice");
                }
            } else
            {
                syslog(LOG_ALERT, "failed to get output of /usr/bin/webservice");
            }
        }
        /* 20 seconds */ 
        usleep(20000000);
    }
    return 0;
}
