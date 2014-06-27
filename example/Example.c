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

DomoDevice dev2 = attachDomoDevice ("DomoSy-Arduino-1633-3668");
if (dev.fd != -1 && dev2.fd != -1 )
{
setPinModeAsOutput (&dev, 22);
setPinModeAsInput (&dev2, 22);
setPinModeAsOutput (&dev2, 23);
int i = 0;
/*for (i = 0; i < 5; i ++)
{
setPinHigh (&dev, 23);
setPinLow (&dev2, 23);
sleep(2);

setPinHigh (&dev2, 23);
setPinLow (&dev, 23);
sleep(2);



}*/
setPinHigh (&dev, 22);

printf("Pin status=%d\n", getPinStatus(&dev2, 22));
 if (getPinStatus(&dev2, 22)==1) 
sleep(1);
setPinHigh (&dev2, 23);
sleep(2);
setPinLow (&dev, 23);



detachDomoDevice (&dev);
detachDomoDevice (&dev2);
}
else
{
printf("Can not open UUGear device.\n");	
}

//cleanupDomo();

    return 0;
}
