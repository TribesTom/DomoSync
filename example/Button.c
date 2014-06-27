#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <mqueue.h>
#include <unistd.h>

#include "../src/Domo.h"

int main(int argc, char **argv)
{

    setShowLogs(1);

    DomoDevice dev = attachDomoDevice ("DomoSy-Arduino-5141-9267");


    if (dev.fd != -1 )
    {
        setPinAsButton(&dev, pinMap[12]);
        char* response;
        int errorcode;
        response=waitForStringBlock(&dev, &errorcode);
        if (atoi(response[0])==12)
        {
            setPinModeAsOutput (&dev, pinMap[13]); // for debug
			sleep(1);
            setPinHigh (&dev, pinMap[13]);
            usleep(0.5*1000*1000);
            setPinLow (&dev, pinMap[13]);
        }
    }
    else
    {
        printf("Can not open device.\n");
    }

	detachDomoDevice (&dev);


    return 0;
}

