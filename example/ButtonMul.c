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

    setShowLogs(1);

    DomoDevice dev = attachDomoDevice ("DomoSy-Arduino-1633-3668");


    if (dev.fd != -1 )
    {
      setMasterButton(&dev, pinMap[22]);
      setPinAsButton(&dev, pinMap[22]);
      setPinAsTorch(&dev,pinMap[24]);
      setPinAsTorch(&dev,pinMap[25]);
      setPinAsTorch(&dev,pinMap[26]);
      setButTor(&dev,pinMap[22],pinMap[24]);
      setButTor(&dev,pinMap[22],pinMap[25]);
      setButTor(&dev,pinMap[22],pinMap[26]);
      


      char* response;
        int errorcode;
        int loop=5;

	while(loop > 0 )
	  {
	response=waitForStringBlock(&dev, &errorcode);
	printf("Changement : %d",loop);
	loop=loop-1;
	  }

	remButTor(&dev,pinMap[22],pinMap[24]);
	loop=5;
        while(loop > 0 )
          {
        response=waitForStringBlock(&dev, &errorcode);
        printf("Changement : %d",loop);
        loop=loop-1;
	}



        
        
    }
    else
    {
        printf("Can not open device.\n");
    }

	detachDomoDevice (&dev);


    return 0;
}

