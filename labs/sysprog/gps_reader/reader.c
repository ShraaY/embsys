#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>

#include <util.h>


#include <syslog.h>

//-----------------------------------------------------------------------------
int main(int argc, char *argv [])
{
    char * port = NULL;
    char * port2 = NULL;

    // parse comand line
    if (argc != 5) //nombre d'args
    {
        fprintf(stderr, "Invalid usage: reader -p port_name\n");
        exit(EXIT_FAILURE);
    }

    char * options = "p:s:";
    int option;
    while((option = getopt(argc, argv, options)) != -1)
    {
        switch(option)
        {
            case 'p':
                port = optarg;
                break;

            case 's':
                port2 =  optarg;
                break;

            case '?':
                fprintf(stderr, "Invalid option %c\n", optopt);
                exit(EXIT_FAILURE);
        }
    }

    // open serial port
    int fd = open(port, O_RDWR | O_NOCTTY);
    if (fd == -1)
    {
        perror("open");
        exit(EXIT_FAILURE);
    }
    tcflush(fd, TCIOFLUSH);

    int fd2 = open(port2, O_RDWR | O_NOCTTY);
    if (fd2 == -1)
    {
        perror("open");
        exit(EXIT_FAILURE);
    }
    tcflush(fd2, TCIOFLUSH);

    int ft = fd;

    // read port
    char buff[50];
    char buff2[50];
    fd_set fdset;

    //open syslog
    openlog ("time", LOG_CONS | LOG_PID | LOG_CRON, LOG_LOCAL1);

    while(1)
    {
        bzero(buff, sizeof(buff));
        FD_ZERO(&fdset);
        FD_SET(fd, &fdset);

        bzero(buff2, sizeof(buff2));
        FD_SET(fd2, &fdset);

        if (fd <= fd2)
        {
            ft = fd2;
        }else{
            ft = fd;
        }
        select(ft+1, &fdset, NULL, NULL, NULL);

        if (FD_ISSET(fd, &fdset))
        {
            int bytes = read (fd, buff, sizeof(buff));

            if (bytes > 0)
            {
                printf("port 1: %s\n", buff);
                fflush(stdout);
            }
        }

        if (FD_ISSET(fd2, &fdset))
        {
            int bytes2 = read (fd2, buff2, sizeof(buff2));

            if (bytes2 > 0)
            {
                printf("port 2: %s\n", buff2);
                fflush(stdout);
            }
        }



    //syslog(1, "aaaa"); //ds console /var/log tail /var/log/syslog


    }

    // close serial port
    close(fd);
    close(fd2);

    closelog();


    exit(EXIT_SUCCESS);
}
