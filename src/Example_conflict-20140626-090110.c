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

	DomoDevice dev = attachDomoDevice ("DomoSy-Arduino-7853-2668");

	if (dev.fd != -1)
	{
		setPinModeAsOutput (&dev, 13);
		int i = 0;
		for (i = 0; i < 5; i ++)
		{
			setPinHigh (&dev, 13);

			usleep(200000);

			setPinLow (&dev, 13);

			usleep(200000);
		}

	setPinModeAsInput (&dev, 9);

	printf("Pin 9 status=%d\n", getPinStatus(&dev, 9));

	sleep(1);

	detachDomoDevice (&dev);
	}
	else
	{
	printf("Can not open UUGear device.\n");	
	}

cleanupUUGear();

    return 0;
}