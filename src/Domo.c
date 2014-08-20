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
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <mqueue.h>
#include <sys/wait.h>
#include <errno.h>
#include <time.h>
#include <stdarg.h>

#include "Domo.h"


int showLogs = 1;

void setShowLogs (int show)
{
	showLogs = show;
}

void printLog (char* fmt, ...)
{
	if (showLogs)
	{
	    va_list args;
	    va_start(args,fmt);
	    vprintf(fmt,args);
	    va_end(args);
	}
}

static void ASSERT_TRUE(int x)
{
	if(x) return ;
	else exit(EXIT_FAILURE);
}


DomoDevice internalAttachDevice (char *id, int printError)
{
	DomoDevice dev;
	strcpy (dev.id, id);
	dev.clientId = -1;
	dev.fd = -1;
	
	/* input */
	mqd_t in = mq_open (REQUEST_QUEUE_NAME, O_WRONLY);
    ASSERT_TRUE ((mqd_t)-1 != in);
    dev.in = in;
    
    /* output (prefix + clientId) */
    struct mq_attr attr;
		attr.mq_flags = 0;
	    attr.mq_maxmsg = 10;
	    attr.mq_msgsize = MAX_MSG_SIZE;
	    attr.mq_curmsgs = 0;
    int clientId;
    char queueName[strlen (RESPONSE_QUEUE_PREFIX) + 4];
    for (clientId = 1; clientId < 256; clientId ++)
    {
	    sprintf (queueName, "%s%d", RESPONSE_QUEUE_PREFIX, clientId);
	    
	    mqd_t out = mq_open (queueName, O_CREAT | O_EXCL | O_RDONLY, 0644, &attr);
	    if ((mqd_t)-1 != out)
	    {
	    	printLog ("Client queue name=%s\n", queueName);
	    	dev.clientId = clientId;
	    	dev.out = out;
	    	break;
	    }
	}
	
	/* if queue is not created, exit */
	ASSERT_TRUE (dev.clientId != -1);
	
	/* construct and send message */
	char buffer[MAX_MSG_SIZE + 1];
	sprintf (buffer,"%d%s%d%s%d%s%s", MSG_GET_DEVICE, MSG_PART_SEPARATOR, clientId, MSG_PART_SEPARATOR, -1, MSG_PART_SEPARATOR, id);
	
	ASSERT_TRUE (0 <= mq_send (in, buffer, strlen (buffer), 0));
	
	/* wait for the response */
	int bytes = 0;
	struct timespec ts;
    ts.tv_sec = time(0) + 2;
    ts.tv_nsec = 0;
    
	if ((bytes = mq_timedreceive (dev.out, buffer, MAX_MSG_SIZE, NULL, &ts)) == -1)
    {
    	if (printError)
    	{
	        if (errno == ETIMEDOUT)
	        {
	        	printLog ("Domo device not found before timeout.\n");
	        }
	        else
	        {
	        	printLog ("Domo device not found. Error=%d.\n", errno);
	        }
    	}
        /* device not found, release the queue name */
		mq_unlink (queueName);
    }
    else
    {
    	buffer[bytes] = 0;
    	dev.fd = atoi (buffer);
    	if (dev.fd == -1)
    	{
    		printLog ("Device has been attached already.\n");
    	}
    	else
    	{
			printLog ("Domo device found. fd=%d\n", dev.fd);
		}
    }
	return dev;
}

DomoDevice attachDomoDevice (char *id)
{
	DomoDevice dev = internalAttachDevice (id, 0);
	if (dev.fd == -1)
	{
		/* newly connected device may need a second shoot */
		sleep (1);
		dev = internalAttachDevice (id, 1);
	}
	return dev;
}

void sendMessage(mqd_t in, int msgType, int clientId, int fd, int pin)
{
	char buffer[MAX_MSG_SIZE + 1];
	sprintf (buffer,"%d%s%d%s%d%s%d", msgType, MSG_PART_SEPARATOR, clientId, MSG_PART_SEPARATOR, fd, MSG_PART_SEPARATOR, pin);
	ASSERT_TRUE (0 <= mq_send (in, buffer, strlen (buffer), 0));
}

void sendMessageWithParameter(mqd_t in, int msgType, int clientId, int fd, int pin, int parameter)
{
	char buffer[MAX_MSG_SIZE + 1];
	sprintf (buffer,"%d%s%d%s%d%s%d%s%d", msgType, MSG_PART_SEPARATOR, clientId, MSG_PART_SEPARATOR, fd, MSG_PART_SEPARATOR, pin, MSG_PART_SEPARATOR, parameter);
	ASSERT_TRUE (0 <= mq_send (in, buffer, strlen (buffer), 0));
}

void setPinModeAsOutput(DomoDevice *dev, int pin)
{
	sendMessage(dev->in, MSG_SET_PIN_OUTPUT, dev->clientId, dev->fd, pin);
}


void setPinModeAsInput(DomoDevice *dev, int pin)
{
	sendMessage(dev->in, MSG_SET_PIN_INPUT, dev->clientId, dev->fd, pin);
}
void setPinModeAsPWM(DomoDevice *dev, int pin)
{
	sendMessage(dev->in, MSG_SET_PIN_PWM, dev->clientId, dev->fd, pin);
}


void setPinHigh(DomoDevice *dev, int pin)
{
	sendMessage(dev->in, MSG_SET_PIN_HIGH, dev->clientId, dev->fd, pin);
}


void setPinLow(DomoDevice *dev, int pin)
{
	sendMessage(dev->in, MSG_SET_PIN_LOW, dev->clientId, dev->fd, pin);
}

char * waitForString(DomoDevice *dev, int * errorCode)
{
	int errCode = 0;
	char * buffer = malloc (MAX_MSG_SIZE + 1);
	int bytes = 0;
	struct timespec ts;
    ts.tv_sec = time(0) + 2;
    ts.tv_nsec = 0;
	if ((bytes = mq_timedreceive (dev->out, buffer, MAX_MSG_SIZE, NULL, &ts)) == -1)
    {
        if (errno == ETIMEDOUT)
        {
        	printLog ("No data was received before timeout.\n");
        	errCode = -1;
        }
        else
        {
        	printLog ("Can not receive data. Error=%d.\n", errno);
        	errCode = -2;
        }
    }
    else
    {
    	buffer[bytes] = 0;
    }
    if (errorCode != NULL)
	{
		*errorCode = errCode;
	}
	return (char *)buffer;
}

char * waitForStringBlock(DomoDevice *dev, int * errorCode)
{
	int errCode = 0;
	char * buffer = malloc (MAX_MSG_SIZE + 1);
	int bytes = 0;
	if ((bytes = mq_receive (dev->out, buffer, MAX_MSG_SIZE, NULL)) == -1)
    {
        
    }
    else
    {
    	buffer[bytes] = 0;
    }
    if (errorCode != NULL)
	{
		*errorCode = errCode;
	}
	return (char *)buffer;
}


float waitForFloat(DomoDevice *dev, int * errorCode)
{
	char * str = waitForString (dev, errorCode);
	float result = atof (str);
	free ((void *)str);
	return result;
}

int waitForInteger(DomoDevice *dev, int * errorCode)
{
	char * str = waitForString (dev, errorCode);
	int result = atoi (str);
	free ((void *)str);
	return result;
}

int getPinStatus(DomoDevice *dev, int pin)
{
	sendMessage(dev->in, MSG_GET_PIN_STATUS, dev->clientId, dev->fd, pin);
	int errorCode = 0;
	int result = waitForInteger(dev, &errorCode);
	return errorCode == 0 ? result : -1;
}


void analogWrite(DomoDevice *dev, int pin, int value) {
	sendMessageWithParameter(dev->in, MSG_ANALOG_WRITE, dev->clientId, dev->fd, pin, value);
}


int analogRead(DomoDevice *dev, int pin)
{
	sendMessage(dev->in, MSG_ANALOG_READ, dev->clientId, dev->fd, pin);
	int errorCode = 0;
	int result = waitForInteger(dev, &errorCode);
	return errorCode == 0 ? result : -1;
}

void analogReference(DomoDevice *dev, int type)
{
	sendMessage(dev->in, MSG_ANALOG_REFERENCE, dev->clientId, dev->fd, type);
}

void setPinAsButton(DomoDevice *dev, int pin)
{
	sendMessage(dev->in, MSG_SET_BUTTON, dev->clientId, dev->fd, pin);
}

void setMasterButton(DomoDevice *dev, int pin)
{
	sendMessage(dev->in, MSG_SET_MASTER, dev->clientId, dev->fd, pin);
}
void setButTor(DomoDevice *dev, int pin, int value) {
        sendMessageWithParameter(dev->in, MSG_SET_BUT_TOR, dev->clientId, dev->fd, pin, value);
}
void remButTor(DomoDevice *dev, int pin, int value) {
        sendMessageWithParameter(dev->in, MSG_REM_BUT_TOR, dev->clientId, dev->fd, pin, value);
}
void setPinAsTorch(DomoDevice *dev, int pin)
{
        sendMessage(dev->in, MSG_SET_TOR, dev->clientId, dev->fd, pin);
}


int readDHT11(DomoDevice *dev, int pin)
{
	sendMessage(dev->in, MSG_READ_DHT11, dev->clientId, dev->fd, pin);
	int errorCode = 0;
	int result = waitForInteger(dev, &errorCode);
	return errorCode == 0 ? result : -3;
}

float readSR04(DomoDevice *dev, int trigPin, int echoPin)
{
	sendMessageWithParameter(dev->in, MSG_READ_SR04, dev->clientId, dev->fd, trigPin, echoPin);
	int errorCode = 0;
	float result = waitForFloat(dev, &errorCode);
	return errorCode == 0 ? result : errorCode;
}


void detachDomoDevice (DomoDevice *dev)
{
	char buffer[MAX_MSG_SIZE + 1];
	sprintf (buffer,"%d%s%d", MSG_CLOSE_DEVICE, MSG_PART_SEPARATOR, dev->clientId);
	ASSERT_TRUE (0 <= mq_send (dev->in, buffer, strlen (buffer), 0));
	
	/* close message queues */
	ASSERT_TRUE ((mqd_t)-1 != mq_close (dev->out));
	ASSERT_TRUE ((mqd_t)-1 != mq_close (dev->in));
	
	/* relese the queue name */
	char queueName[strlen (RESPONSE_QUEUE_PREFIX) + 4];
	sprintf (queueName, "%s%d", RESPONSE_QUEUE_PREFIX, dev->clientId);
	mq_unlink (queueName);
}



void cleanupDomo ()
{
	mqd_t mq = mq_open (REQUEST_QUEUE_NAME, O_WRONLY);
	if ((mqd_t)-1 == mq)
	{
		printLog ("Domo daemon is not running.\n");
	}
	else
	{
		/* stop Domo daemon */
		char cmd[16];
		memset (cmd, 0, sizeof cmd);
		sprintf (cmd,"%d", MSG_EXIT);
		mq_send (mq, cmd, strlen (cmd), 0);
		
		/* release all queue names */
		int clientId;
		char queueName[strlen (RESPONSE_QUEUE_PREFIX) + 4];
	    for (clientId = 1; clientId < 256; clientId ++)
	    {
		    sprintf (queueName, "%s%d", RESPONSE_QUEUE_PREFIX, clientId);
		    mq_unlink (queueName);
		}
	}
}

