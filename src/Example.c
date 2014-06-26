#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <mqueue.h>
#include <unistd.h>

#include "Domo.h"

int main(int argc, char **argv)
{

setShowLogs(1);

DomoDevice dev = attachDomoDevice ("DomoSy-Arduino-8823-3600");

if (dev.fd != -1)
{
setPinModeAsOutput (&dev, 23);
int i = 0;
for (i = 0; i < 5; i ++)
{
setPinHigh (&dev, 23);

sleep(2);

setPinLow (&dev, 23);

sleep(2);
}

setPinModeAsInput (&dev, 47);

printf("Pin 9 status=%d\n", getPinStatus(&dev, 47));

sleep(1);
setPinHigh (&dev, 23);
sleep(2);
setPinLow (&dev, 23);



detachDomoDevice (&dev);
}
else
{
printf("Can not open UUGear device.\n");	
}

//cleanupDomo();

    return 0;
}
