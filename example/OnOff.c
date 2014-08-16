#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <mqueue.h>
#include <unistd.h>
#include "../src/Domo.h"
#include "../src/mappin/mega2560.h"

	
int main(int argc, char **argv)
{
if(argc != 4 ) { 
	printf("Erreur d'argument, utilisation : ./Switch DomoSy-... numPinDigital 0/1\n");
	return -1 ;
	}


setShowLogs(1);

DomoDevice dev = attachDomoDevice (argv[1]);

int pin=pinMap[atoi(argv[2])];
if (dev.fd != -1 )
{
  setPinModeAsOutput (&dev, pin); // for debug

  if (atoi(argv[3])==1) setPinHigh (&dev, pin);
 else  if (atoi(argv[3])==0) setPinLow (&dev, pin);


detachDomoDevice (&dev);
}


    return 0;
}
