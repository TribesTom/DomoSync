/*
 * UUGear Solution: extend your Raspberry Pi with Arduino
 *
 * Author: Shawn (shawn@uugear.com)
 *
 * Copyright (c) 2014 UUGear s.r.o.
 *
 ***********************************************************************
 *  This file is part of UUGear Solution: 
 *  http://www.uugear.com/?page_id=50
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
#include "DomoDaemon.h"

struct DomoDevice
{
	char id[DOMO_ID_MAX_LENGTH];
	int clientId;
	int fd;
	mqd_t in;
	mqd_t out;
};

typedef struct DomoDevice DomoDevice;

extern void setShowLogs (int show);

extern struct DomoDevice attachDomoDevice (char *id);

extern void setPinModeAsOutput(DomoDevice *dev, int pin);

extern void setPinModeAsInput(DomoDevice *dev, int pin);

extern void setPinHigh(DomoDevice *dev, int pin);

extern void setPinLow(DomoDevice *dev, int pin);

extern int getPinStatus(DomoDevice *dev, int pin);

extern void setPinModeAsPWM(DomoDevice *dev, int pin);

extern void analogWrite(DomoDevice *dev, int pin, int value);

extern int analogRead(DomoDevice *dev, int pin);

extern void setPinAsButton(DomoDevice *dev, int pin);

extern void setMasterButton(DomoDevice *dev, int pin);

extern void setButTor(DomoDevice *dev, int pin, int value);

extern void remButTor(DomoDevice *dev, int pin, int value);

extern void setPinAsTorch(DomoDevice *dev, int pin);

extern void setPinTorchUp(DomoDevice *dev, int pin);

extern void setPinTorchDown(DomoDevice *dev, int pin);

extern int getPinTorchStatus(DomoDevice *dev, int pin);

/**
 * Configures the reference voltage used for analog input
 * (i.e. the value used as the top of the input range)
 *
 * parameters:
 *	dev is the pointer of the device struct
 *	type is the reference type:
 *		0 - DEFAULT
 *		1 - EXTERNAL
 */
extern void analogReference(DomoDevice *dev, int type);

extern void detachDomoDevice (DomoDevice *dev);

/*
 * Read humidity and temperature values from DHT11 sensor
 *
 * parameters:
 *	dev is the pointer of the device struct
 *	pin is the digital pin that connects to the DATA wire of DHT11
 *
 * returns:
 *	integer that lowest byte is temperature while higher byte is humidity
 *	-1 for timeout
 *	-2 for checksum error
 *	-3 for communication error (please check /var/log/syslog)
 */
extern int readDHT11(DomoDevice *dev, int pin);

/**
 * Read distance value (in cm) from HC-SR04 sensor
 *
 * parameters:
 *	dev is the pointer of the device struct
 *	trigPin is the digital pin that connects to the TRIG wire of SR04
 *	echoPin is the digital pin that connects to the ECHO wire of SR04
 *  echoPin could be the same than trigPin if you wire them together.
 *
 * returns:
 *	float value of the distance in cm
 *  you may need to check if it is in the correct range by yourself.
 */
extern float readSR04(DomoDevice *dev, int trigPin, int echoPin);

extern char * waitForStringBlock(DomoDevice *dev, int * errorCode);

extern void cleanupDomo ();
