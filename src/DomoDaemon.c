/*
 * DomoSync Solution: connect Linux to Arduino using Cosa libs on Arduino
 *
 * Author: TribesTom (tribestom@tribesconcept.be ) inspired by Shawn (shawn@uugear.com)  http://www.uugear.com/?page_id=50
 *
 *
 * Copyright (c) 2014 UUGear s.r.o.
 *
 ***********************************************************************
 *  This file is part of UUGear Solution:
 *
 *
 *  UUGear Solution is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published
 *  by the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  UUGear Solution is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with UUGear Solution.  If not, see <http://www.gnu.org/licenses/>.
 *
 ***********************************************************************
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <syslog.h>
#include <errno.h>
#include <pwd.h>
#include <signal.h>
#include <time.h>
#include <mqueue.h>
#include <pthread.h>
#include <dirent.h>
#include <libudev.h>
#include <termios.h>
#include <sys/syscall.h>
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr

#include "DomoDaemon.h"
#include "serial.h"

/* array to store all client state (1=running) */
int clientStatus[256];
DeviceOpen *ListDevice=NULL;
Cli CliList[30];

static pthread_mutex_t mutexDevice = PTHREAD_MUTEX_INITIALIZER;

static void child_handler(int signum)
{
    switch (signum) {
    case SIGALRM:
        exit(EXIT_FAILURE);
        break;
    case SIGUSR1:
        exit(EXIT_SUCCESS);
        break;
    case SIGCHLD:
        exit(EXIT_FAILURE);
        break;
    }
}


static void daemonize(const char *lockfile)
{
    pid_t pid, sid, parent;
    int lfp = -1;

    /* Already a daemon */
    if (getppid () == 1) return;

    /* Create the lock file as the current user */
    if (lockfile && lockfile[0]) {
        lfp = open (lockfile,O_RDWR|O_CREAT,0640);
        if (lfp < 0) {
            syslog (LOG_ERR, "Could not create lock file %s, code=%d (%s)",
                    lockfile, errno, strerror (errno));
            exit (EXIT_FAILURE);
        }
    }

    /* Drop user if there is one, and we were run as root */
    if (getuid () == 0 || geteuid () == 0) {
        struct passwd *pw = getpwnam (RUN_AS_USER);
        if (pw) {
            syslog (LOG_NOTICE, "Run daemon as user: " RUN_AS_USER);
            setuid (pw->pw_uid);
        }
    }

    /* Trap signals that we expect to recieve */
    signal (SIGCHLD,child_handler);
    signal (SIGUSR1,child_handler);
    signal (SIGALRM,child_handler);

    /* Fork off the parent process */
    pid = fork ();
    if (pid < 0) {
        syslog (LOG_ERR, "Could not fork daemon, code=%d (%s)",
                errno, strerror (errno));
        exit (EXIT_FAILURE);
    }
    /* If we got a good PID, then we can exit the parent process. */
    if (pid > 0) {

        /* Wait for confirmation from the child via SIGTERM or SIGCHLD, or
           for two seconds to elapse (SIGALRM).  pause() should not return. */
        alarm (2);
        pause ();

        exit (EXIT_FAILURE);
    }

    /* At this point we are executing as the child process */
    parent = getppid ();

    /* Cancel certain signals */
    signal (SIGCHLD,SIG_DFL); /* A child process dies */
    signal (SIGTSTP,SIG_IGN); /* Various TTY signals */
    signal (SIGTTOU,SIG_IGN);
    signal (SIGTTIN,SIG_IGN);
    signal (SIGHUP, SIG_IGN); /* Ignore hangup signal */
    signal (SIGTERM,SIG_DFL); /* Die on SIGTERM */

    /* Change the file mode mask */
    umask (0);

    /* Create a new SID for the child process */
    sid = setsid ();
    if (sid < 0) {
        syslog (LOG_ERR, "unable to create a new session, code %d (%s)",
                errno, strerror (errno));
        exit (EXIT_FAILURE);
    }

    /* Change the current working directory.  This prevents the current
       directory from being locked; hence not being able to remove it. */
    if ((chdir("/")) < 0) {
        syslog (LOG_ERR, "unable to change directory to %s, code %d (%s)",
                "/", errno, strerror (errno));
        exit (EXIT_FAILURE);
    }

    /* Redirect standard files to /dev/null */
    //freopen ("/dev/null", "r", stdin);
    //freopen ("/dev/null", "w", stdout);
    // freopen ("/dev/null", "w", stderr);

    /* Notify the parent process that daemonize is done */
    kill (parent, SIGUSR1);
}


int startsWith(const char *str, const char *pre)
{
    size_t lenpre = strlen (pre),
                    lenstr = strlen (str);
    return lenstr < lenpre ? 0 : strncmp (pre, str, lenpre) == 0;
}


int endsWith(const char *str, const char *suffix)
{
    if (!str || !suffix)
        return 0;
    size_t lenstr = strlen(str);
    size_t lensuffix = strlen(suffix);
    if (lensuffix >  lenstr)
        return 0;
    return strncmp(str + lenstr - lensuffix, suffix, lensuffix) == 0;
}


void responseToClient(int clientId, char *resp)
{
    char queueName[MAX_QUEUE_NAME_LENGTH];
    sprintf (queueName,"%s%d", RESPONSE_QUEUE_PREFIX, clientId);
    mqd_t mq = mq_open (queueName, O_WRONLY);
    if ((mqd_t)-1 != mq) {
        mq_send (mq, resp, strlen (resp), 0);
        mq_close (mq);
    }
}


void *deviceOpener(void *arg)
{
    pthread_detach(pthread_self());
    int thID=(int)syscall(SYS_gettid);
    DeviceOpen *devOpen = (DeviceOpen *)arg;
    syslog (LOG_INFO, "Thread Device %d : Started device opener for: %s fd : %d ",thID, devOpen->devName,devOpen->fd );
    int i = 0;
    int j = 0;
    int head = -1;
    int chr;
    char buf[DOMO_ID_MAX_LENGTH + 1];
    memset (buf, 0, sizeof(buf));
    if (devOpen==NULL) {
        syslog (LOG_INFO, "Thread Device %d : Error : Started device opener NULL",thID);
    }

    // wait for device ready ( device reboot on usb open )
    sleep(2);
    // read response from serial
    while ((chr = serialReadChar (devOpen->fd)) > 0) {
        if (head == -1 && chr == DOMO_RESPONSE_START_CHAR) {
            head = chr;
        } else if (head == DOMO_RESPONSE_START_CHAR) {

            buf[j ++] = (chr & 0xFF);
            if (endsWith (buf, DOMO_RESPONSE_END_STRING)) {
                break;
            }

        }
    }
    syslog (LOG_INFO, "Thread Device %d : Looping device opener for : %s buffer: %s taille buf : %d ",thID,devOpen->devName, buf,j);
    // extract device name
    if (j>0 && startsWith (buf, DOMO_ID_PREFIX)) {

        syslog (LOG_INFO, "Thread Device %d : Looking device name : %s",thID,buf);
        char *end = strstr (buf, DOMO_RESPONSE_END_STRING);
        if (end != NULL) {
            buf[end - buf] = 0;
        }
        syslog (LOG_INFO, "Thread Device %d : Receive device id: %s",thID, buf);
        devOpen->id = malloc (strlen(buf) + 1);
        strcpy (devOpen->id, buf);

        syslog (LOG_INFO, "Thread Device %d : Device found, send fd=%d",thID,devOpen->fd);

        // device found, send back the fd
        char fdBuf[16];
        memset (fdBuf, 0, sizeof fdBuf);
        sprintf (fdBuf,"%d", devOpen->fd);

        struct termios attribs;
        // loop to keep reading response for other commands
        while ( devOpen->fd >= 0 && tcgetattr(devOpen->fd,&attribs) == 0) {
            // read response from serial

            int k = 0;
            int hd = -1;
            int cid = -1;
            memset (buf, 0, sizeof buf);
            while ((chr = serialReadChar (devOpen->fd)) > 0) {
                if (hd == -1 && chr == DOMO_RESPONSE_START_CHAR) {
                    hd = chr;
                } else if (hd == DOMO_RESPONSE_START_CHAR) {
                    if (cid == -1) {
                        cid = chr;
                    } else {
                        buf[k ++] = (chr & 0xFF);
                        if (endsWith (buf, DOMO_RESPONSE_END_STRING)) {
                            break;
                        }
                    }
                }
            }
            if (k > 0) {
                char *end = strstr (buf, DOMO_RESPONSE_END_STRING);
                if (end != NULL) {
                    buf[end - buf] = 0;
                }
                syslog (LOG_INFO, "Thread Device %d : Received response from %s(fd=%d): %s Client : %d ",thID,devOpen->devName, devOpen->fd, buf,cid);




                // send back the response to client
                mq_send (CliList[cid].mq, buf, strlen (buf), 0);
            }
        }

        // cleanup
        
    }
    serialClose (devOpen->fd);
    syslog (LOG_INFO, "Thread Device %d : Terminated device opener for: %s (fd=%d)",thID,devOpen->devName, devOpen->fd);
    pthread_mutex_lock (&mutexDevice);
    if (devOpen->prev != NULL && devOpen->next != NULL ) {
        devOpen->prev->next = devOpen->next;
        devOpen->next->prev = devOpen->prev;
    }
    else if (devOpen->prev == NULL && devOpen->next != NULL )  ListDevice = devOpen->next;
    else ListDevice = NULL;
    if (devOpen->devName != NULL ) free (devOpen->devName);
    if (devOpen != NULL) free (devOpen);
    pthread_mutex_unlock(&mutexDevice);
    syslog (LOG_INFO, "Thread Device %d : Terminated FREE device",thID);
    return NULL;
}

void sendCommand(char cmd, int clientId, int targetFd, int pin)
{
    syslog (LOG_INFO, "Send command: cmd=0x%02x, clientId=%d, fd=%d, pin=%d", cmd, clientId, targetFd, pin);
    char command[7] = { COMMAND_START_CHAR, cmd, (char)(pin & 0xFF), (char)(clientId & 0xFF), (COMMAND_END_CHAR1 & 0xFF), (COMMAND_END_CHAR2 & 0xFF), 0x00 };
    serialWriteData (targetFd, command, 7);
    syslog ( LOG_INFO, "Sent %d;%d;%d;%d;%d;%d \n",command[0],command[1],command[2],command[3],command[4],command[5]);
}

void sendCommandWithParameter(char cmd, int clientId, int targetFd, int pin, int parameter)
{
    syslog (LOG_INFO, "Send command: cmd=0x%02x, clientId=%d, fd=%d, pin=%d, parameter=%d", cmd, clientId, targetFd, pin, parameter);
    char command[8] = { COMMAND_START_CHAR, cmd, (char)(pin & 0xFF), (char)(parameter & 0xFF), (char)(clientId & 0xFF), (char)COMMAND_END_CHAR1, (char)COMMAND_END_CHAR2, 0x00 };
    serialWriteData (targetFd, command, 8);
}
void openMultipleDevice()
{
    syslog (LOG_INFO, "Try to Open New Device");
    DIR *baseDir;
    struct dirent *dir;
    baseDir = opendir ("/dev");
    if (baseDir) {
        while ((dir = readdir (baseDir)) != NULL) {
            if (dir->d_type == DT_CHR) {
                if (startsWith (dir->d_name, DOMO_DEVICE_PREFIX1) || startsWith (dir->d_name, DOMO_DEVICE_PREFIX2)) {
                    // open the device for id query
                    char* devicePath=malloc(32*sizeof(char));
                    strcpy (devicePath, "/dev/");
                    strcat (devicePath, dir->d_name);
                    char* name= malloc(16*sizeof(char));
                    strcpy (name, dir->d_name);
                    openOneDevice(devicePath, name);
                }
            }
        }
        closedir (baseDir);
    }
}

int openOneDevice(char* devicePath, char* d_name)
{
    int fd = serialOpen (devicePath, DOMO_DEVICE_BAUD_RATE);
    int thID=(int)syscall(SYS_gettid);
    if (fd > 0) {
        syslog (LOG_INFO, "Thread Device %d : Opened serial device: %s (fd=%d)\n",thID,d_name, fd);
        // Start a device opener
        DeviceOpen *devOpen;
        devOpen = malloc (sizeof(*devOpen));
        devOpen->fd = fd;
        devOpen->devName = malloc (strlen (d_name) + 1);
        devOpen->prev = NULL;
        devOpen->next = NULL;
        strcpy (devOpen->devName, d_name);
        // Add to list
        pthread_mutex_lock(&mutexDevice);
        if ( ListDevice == NULL ) ListDevice = devOpen;
        else {
            DeviceOpen *DevicePointer = ListDevice;
            while ( DevicePointer->next != NULL ) DevicePointer = DevicePointer->next;
            DevicePointer->next = devOpen;
            devOpen->prev=DevicePointer;
        }
        pthread_mutex_unlock(&mutexDevice);

        // Launch Thread
        pthread_t doThread;
        pthread_create (&doThread, NULL, deviceOpener, (void *)devOpen);

        // write "get ID" command to serial
        char getIdCmd[] = { COMMAND_START_CHAR, CMD_GET_DEVICE_ID, COMMAND_END_CHAR1, COMMAND_END_CHAR2, 0x00 };

        // Wait for device reboot on serial open
        sleep(1);
        serialWriteString (fd, getIdCmd);
        syslog (LOG_INFO, "Thread Device %d : Send getID command for deive: %s command : %d,%d,%d,%d,%d\n",thID, devOpen->devName,getIdCmd[0],getIdCmd[1],getIdCmd[2],getIdCmd[3],getIdCmd[4]);
        serialFlush (fd);
        return 1;
    } else {
        syslog (LOG_INFO, "Thread Device %d : Could not open serial deive: %s\n",thID, d_name);
        return -1;
    }
}
int searchTarget( char *part )
{
    pthread_mutex_lock(&mutexDevice);

    if ( ListDevice != NULL )
    {
        DeviceOpen *devPointeur = ListDevice;
        while ( strcmp(part,devPointeur->id ) !=0 )
        {
            if ( devPointeur->next != NULL) devPointeur= devPointeur->next;
            else {
                pthread_mutex_unlock(&mutexDevice);
                return -1;
            }
        }
        pthread_mutex_unlock(&mutexDevice);
        return devPointeur->fd;
    }
    pthread_mutex_unlock(&mutexDevice);
    return -1;
}
static void ASSERT_TRUE(int x)
{
    if (x) return ;
    else {
        printf("error: %s (%d)\n", strerror(errno), errno);
        printf("Assert False \n");
        exit(EXIT_FAILURE);
    }
}
void *deviceMonitoring(void* arg)
{
    pthread_detach(pthread_self());
    int thID=(int)syscall(SYS_gettid);
    struct udev_monitor *mon;
    struct udev *udev;
    struct udev_device *dev;
    int fd;
    udev = udev_new();
    if (!udev) {
        syslog(LOG_INFO,"Thread UDEV %d : Can't create udev\n",thID);
        exit(1);
    }
    syslog (LOG_INFO,"Thread UDEV %d : Udev Monitoring device",thID);
    mon = udev_monitor_new_from_netlink(udev, "udev");
    udev_monitor_filter_add_match_subsystem_devtype(mon, "tty", NULL);
    udev_monitor_enable_receiving(mon);
    /* Get the file descriptor (fd) for the monitor.
       This fd will get passed to select() */
    fd = udev_monitor_get_fd(mon);

    /* This section will run continuously, calling usleep() at
       the end of each pass. This is to demonstrate how to use
       a udev_monitor in a non-blocking way. */
    while (1) {
        /* Set up the call to select(). In this case, select() will
           only operate on a single file descriptor, the one
           associated with our udev_monitor. Note that the timeval
           object is set to 0, which will cause select() to not
           block. */
        fd_set fds;
        struct timeval tv;
        int ret;

        FD_ZERO(&fds);
        FD_SET(fd, &fds);
        tv.tv_sec = 0;
        tv.tv_usec = 0;

        ret = select(fd+1, &fds, NULL, NULL, &tv);

        /* Check if our file descriptor has received data. */
        if (ret > 0 && FD_ISSET(fd, &fds)) {
            // printf("\nselect() says there should be data\n");

            /* Make the call to receive the device.
               select() ensured that this will not block. */
            dev = udev_monitor_receive_device(mon);
	    usleep(250*1000);
            if (dev) {
                const char* szAction = udev_device_get_action(dev);
                if (strcmp("add",szAction) ==0 ) {
                    char* devicePath=malloc(32*sizeof(char));;
                    strcpy(devicePath,udev_device_get_devnode(dev));
                    char* d_name=malloc(16*sizeof(char));;
                    strcpy(d_name,udev_device_get_sysname(dev));
                    syslog (LOG_INFO,"Thread UDEV %d : Udev path : %s name : %s\n",thID, devicePath,d_name);
                    sleep(2);
                    openOneDevice(devicePath,d_name);
		    free(devicePath);
		    free(d_name);
                }
            }
            else {
                syslog (LOG_INFO,"Udev :Thread UDEV %d: Device from receive_device(). An error occured.\n",thID);
            }
        }
        usleep(250*1000);

        fflush(stdout);
    }
    udev_device_unref(dev);
}




/*
 * This will handle connection for each client
 * */
void *connection_handler(void *socket_desc)
{
    pthread_detach(pthread_self());
    int thID=(int)syscall(SYS_gettid);
    //Get the socket descriptor
    int sock = *(int*)socket_desc;
    char d_name[30];
	
	
	// generate devopen device
    sprintf (d_name,"%s%d", ETHERNET_PREFIX, thID);
    DeviceOpen *devOpen;
    devOpen = malloc (sizeof(*devOpen));
    devOpen->fd = sock;
    devOpen->devName = malloc (strlen (d_name) + 1);
    devOpen->prev = NULL;
    devOpen->next = NULL;
    strcpy (devOpen->devName, d_name);
    
	// Add to list
    pthread_mutex_lock(&mutexDevice);
    if ( ListDevice == NULL ) ListDevice = devOpen;
    else {
        DeviceOpen *DevicePointer = ListDevice;
        while ( DevicePointer->next != NULL ) DevicePointer = DevicePointer->next;
        DevicePointer->next = devOpen;
        devOpen->prev=DevicePointer;
    }
	pthread_mutex_unlock(&mutexDevice);
	sleep(3);
	// Get device ID
    char getIdCmd[] = { COMMAND_START_CHAR, CMD_GET_DEVICE_ID, COMMAND_END_CHAR1, COMMAND_END_CHAR2, 0x00 };
    serialWriteString (devOpen->fd, getIdCmd);
    syslog (LOG_INFO, "Thread Device %d : Send getID command for deive: %s command : %d,%d,%d,%d,%d socket %d\n",thID, d_name,getIdCmd[0],getIdCmd[1],getIdCmd[2],getIdCmd[3],getIdCmd[4],devOpen->fd);
    
	int i = 0;
    int j = 0;
    int head = -1;
    int chr;
    char buf[DOMO_ID_MAX_LENGTH + 1];
    memset (buf, 0, sizeof buf);
    if (devOpen==NULL) {
        syslog (LOG_INFO, "Thread Device %d : Error : Started device opener NULL",thID);
    }

       
    // read response from device
    while ((chr = serialReadChar (devOpen->fd)) > 0) {
        if (head == -1 && chr == DOMO_RESPONSE_START_CHAR) {
            head = chr;
        } else if (head == DOMO_RESPONSE_START_CHAR) {

            buf[j ++] = (chr & 0xFF);
            if (endsWith (buf, DOMO_RESPONSE_END_STRING)) {
                break;
            }

        }
    }
    syslog (LOG_INFO, "Thread Device %d : Looping device opener for : %s buffer: %s taille buf : %d ",thID,devOpen->devName, buf,j);
    // extract device name
    if (startsWith (buf, DOMO_ID_PREFIX)) {

        syslog (LOG_INFO, "Thread Device %d : Looking device name : %s",thID,buf);
        char *end = strstr (buf, DOMO_RESPONSE_END_STRING);
        if (end != NULL) {
            buf[end - buf] = 0;
        }
        syslog (LOG_INFO, "Thread Device %d : Receive device id: %s",thID, buf);
        devOpen->id = malloc (strlen(buf) + 1);
        strcpy (devOpen->id, buf);

        syslog (LOG_INFO, "Thread Device %d : Device found, send fd=%d",thID,devOpen->fd);

        // device found, send back the fd
        char fdBuf[16];
        memset (fdBuf, 0, sizeof fdBuf);
        sprintf (fdBuf,"%d", devOpen->fd);

       
        // loop to keep reading response for other commands
        while ( devOpen->fd >= 0 ) {
            // read response from serial

            int k = 0;
            int hd = -1;
            int cid = -1;
            memset (buf, 0, sizeof buf);
            while ((chr = serialReadChar (devOpen->fd)) > 0) {
                if (hd == -1 && chr == DOMO_RESPONSE_START_CHAR) {
                    hd = chr;
                } else if (hd == DOMO_RESPONSE_START_CHAR) {
                    if (cid == -1) {
                        cid = chr;
                    } else {
                        buf[k ++] = (chr & 0xFF);
                        if (endsWith (buf, DOMO_RESPONSE_END_STRING)) {
                            break;
                        }
                    }
                }
            }
            if (k > 0) {
                char *end = strstr (buf, DOMO_RESPONSE_END_STRING);
                if (end != NULL) {
                    buf[end - buf] = 0;
                }
                syslog (LOG_INFO, "Thread Device %d : Received response from %s(fd=%d): %s Client : %d ",thID,devOpen->devName, devOpen->fd, buf,cid);
                // send back the response to client
                mq_send (CliList[cid].mq, buf, strlen (buf), 0);
            }
        }        
    }
	serialClose (devOpen->fd);
    syslog (LOG_INFO, "Thread Device %d : Terminated device opener for: %s (fd=%d)",thID,devOpen->devName, devOpen->fd);
    pthread_mutex_lock (&mutexDevice);
    if (devOpen->prev != NULL && devOpen->next != NULL ) {
        devOpen->prev->next = devOpen->next;
        devOpen->next->prev = devOpen->prev;
    }
    else if (devOpen->prev == NULL && devOpen->next != NULL )  ListDevice = devOpen->next;
    else ListDevice = NULL;
    pthread_mutex_unlock(&mutexDevice);
    free (devOpen->devName);
    free (devOpen);
    
    syslog (LOG_INFO, "Thread Device %d : Terminated FREE device",thID);
    
    
    //Free the socket pointer
    free(socket_desc);

    return NULL;
}

void *deviceEthernetMonitoring(void* arg)
{
    pthread_detach(pthread_self());
    int thID=(int)syscall(SYS_gettid);
    int socket_desc , new_socket , c , *new_sock;
    struct sockaddr_in server , client;
    char *message;

    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        syslog(LOG_INFO,"Thread %d : Could not create socket",thID);
    }

    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 8888 );

    //Bind
    if ( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        syslog(LOG_INFO,"Thread %d : bind failed",thID);
        return;
    }

    //Listen
    listen(socket_desc , 3);

    //Accept and incoming connection
    syslog(LOG_INFO,"Thread %d : Waiting for incoming connections...",thID);
    c = sizeof(struct sockaddr_in);
    while ( (new_socket = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
    {
      syslog(LOG_INFO,"Thread %d : Connection accepted",thID);

        //Reply to the client

        pthread_t sniffer_thread;
        new_sock = malloc(1);
        *new_sock = new_socket;

        if ( pthread_create( &sniffer_thread , NULL ,  connection_handler , (void*) new_sock) < 0)
        {
            syslog(LOG_INFO,"Thread %d : could not create thread",thID);
            return ;
        }



    }

    if (new_socket<0)
    {
        syslog(LOG_INFO,"Thread %d : accept failed",thID);
        return NULL;
    }

    return ;
}


int main(int argc, char **argv)
{
    /* initialize the logging system */
    openlog (DAEMON_NAME, LOG_PID, LOG_LOCAL5);
    syslog (LOG_INFO, "Starting DomoSync daemon...");
    /* daemonize */
    daemonize ("/var/lock/" DAEMON_NAME);
    /* we are DomoSync daemon now */

    /* create the message queue */

    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;

    mode_t mode  = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
    mqd_t mq = mq_open (REQUEST_QUEUE_NAME, O_CREAT | O_RDONLY, mode, &attr);
    ASSERT_TRUE ((mqd_t)-1 != mq);

    //Monitor connecting device
    pthread_t doThread;
    pthread_create (&doThread, NULL, deviceMonitoring, NULL);

    // Monitoring Ethernet Device

    pthread_t doThread2;
    pthread_create (&doThread2, NULL, deviceEthernetMonitoring, NULL);
    // Connect to devices
    openMultipleDevice();
    /* main loop */

    char buffer[MAX_MSG_SIZE + 1];
    int must_stop = 0;
    while (!must_stop) {
        ssize_t bytes_read = 0;
        //printf("Loop Launch\n");
        /* receive message */
        bytes_read = mq_receive (mq, buffer, MAX_MSG_SIZE, NULL);
        //ASSERT_TRUE (bytes_read >= 0);

        buffer[bytes_read] = '\0';
        if (strlen (buffer) > 0) {
            syslog (LOG_INFO, "Received: %s", buffer);

            char *parts[MAX_MSG_PARTS];
            int count = 0;
            parts[count] = strtok (buffer, MSG_PART_SEPARATOR);
            while (parts[count] != NULL) {
                parts[++count] = strtok (NULL, MSG_PART_SEPARATOR);
            }
            if (count > 0) {
                int cmd = atoi (parts[0]);
                int clientId = count > 1 ? (atoi (parts[1]) & 0xFF) : -1;
                int targetFd = count > 2 ? (atoi (parts[2]) & 0xFF) : -1;
                switch (cmd) {
                case MSG_EXIT:
                    must_stop = 1;
                    memset (clientStatus, 0, sizeof clientStatus);
                    break;
                    // Create Client
                case MSG_GET_DEVICE:
                    clientStatus[clientId] = 1;
                    CliList[clientId].id=clientId;
                    sprintf (CliList[clientId].messageQueueName,"%s%d", RESPONSE_QUEUE_PREFIX, clientId);
                    CliList[clientId].mq = mq_open (CliList[clientId].messageQueueName, O_WRONLY);
                    int fdTarget = searchTarget(parts[3]);
                    char fdBuf[16];
                    memset (fdBuf, 0, sizeof fdBuf);
                    sprintf (fdBuf,"%d", fdTarget);
                    if (CliList[clientId].mq != (mqd_t) -1 ) mq_send (CliList[clientId].mq, fdBuf , strlen (fdBuf), 0);
                    break;
                    //Close Client
                case MSG_CLOSE_DEVICE:
                    clientStatus[clientId] = 0;
                    CliList[clientId].id=0;
                    mq_close(CliList[clientId].mq);
                    break;
                case MSG_SET_PIN_OUTPUT:
                    sendCommand(CMD_SET_PIN_OUTPUT, clientId, targetFd, count > 3 ? atoi(parts[3]) : -1);
                    break;
                case MSG_SET_PIN_INPUT:
                    sendCommand(CMD_SET_PIN_INPUT, clientId, targetFd, count > 3 ? atoi(parts[3]) : -1);
                    break;
                case MSG_SET_PIN_HIGH:
                    sendCommand(CMD_SET_PIN_HIGH, clientId, targetFd, count > 3 ? atoi(parts[3]) : -1);
                    break;
                case MSG_SET_PIN_LOW:
                    sendCommand(CMD_SET_PIN_LOW, clientId, targetFd, count > 3 ? atoi(parts[3]) : -1);
                    break;
                case MSG_GET_PIN_STATUS:
                    sendCommand(CMD_GET_PIN_STATUS, clientId, targetFd, count > 3 ? atoi(parts[3]) : -1);
                    break;
                case MSG_ANALOG_WRITE:
                    sendCommandWithParameter(CMD_ANALOG_WRITE, clientId, targetFd,
                                             count > 3 ? (atoi (parts[3]) & 0xFF) : -1,
                                             count > 4 ? (atoi (parts[4]) & 0xFF) : -1);
                    break;
                case MSG_ANALOG_READ:
                    sendCommand(CMD_ANALOG_READ, clientId, targetFd, count > 3 ? atoi(parts[3]) : -1);
                    break;
                case MSG_ANALOG_REFERENCE:
                    sendCommand(CMD_ANALOG_REFERENCE, clientId, targetFd, count > 3 ? atoi(parts[3]) : -1);
                    break;
                case MSG_SET_MASTER:
                    sendCommand(CMD_SET_MASTER, clientId, targetFd, count > 3 ? atoi(parts[3]) : -1);
                    break;
                case MSG_SET_BUTTON:
                    sendCommand(CMD_SET_BUTTON, clientId, targetFd, count > 3 ? atoi(parts[3]) : -1);
                    break;
		    /*case MSG_SET_PIN_PWM:
                    sendCommand(CMD_SET_ANALOG, clientId, targetFd, count > 3 ? atoi(parts[3]) : -1);
                    break;*/
                case MSG_READ_DHT11:
                    sendCommand(CMD_READ_DHT11, clientId, targetFd, count > 3 ? atoi(parts[3]) : -1);
                    break;
                case MSG_READ_SR04:
                    sendCommandWithParameter(CMD_READ_SR04, clientId, targetFd,
                                             count > 3 ? (atoi (parts[3]) & 0xFF) : -1,
                                             count > 4 ? (atoi (parts[4]) & 0xFF) : -1);
                    break;
		case MSG_SET_BUT_TOR:
		  sendCommandWithParameter(CMD_SET_BUT_TOR, clientId, targetFd, count > 3 ? atoi(parts[3]) : -1,count > 4 ? atoi(parts[4]) : -1);
                    break;
                case MSG_REM_BUT_TOR:
		  sendCommandWithParameter(CMD_REM_BUT_TOR, clientId, targetFd, count > 3 ? atoi(parts[3]) : -1,count > 4 ? atoi(parts[4]) : -1);
                    break;
		case MSG_SET_TOR:
                    sendCommand(CMD_SET_TOR, clientId, targetFd, count > 3 ? atoi(parts[3]) : -1);
                    break;
                }
            }
        }
    }
    printf("Fin DomoSync");
    /* cleanup before exit */
    ASSERT_TRUE ((mqd_t)-1 != mq_close (mq));
    ASSERT_TRUE ((mqd_t)-1 != mq_unlink (REQUEST_QUEUE_NAME));
    syslog (LOG_NOTICE, "DomoSync daemon is terminated.");
    closelog ();

    return 0;
}

