/*
 * DomoSync Solution: extend your Raspberry Pi with Arduino
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
#ifndef DOMO_DAEMON_H_
#define DOMO_DAEMON_H_

#define DAEMON_NAME "Domo_daemon"

#define RUN_AS_USER "pi"

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

#define REQUEST_QUEUE_NAME  	"/domo_request_queue"
#define RESPONSE_QUEUE_PREFIX	"/domo_response_queue_"
#define MAX_QUEUE_NAME_LENGTH	32
#define MAX_MSG_PARTS		8
#define MAX_MSG_SIZE    	2048
#define MSG_PART_SEPARATOR	"]=["
#define ETHERNET_PREFIX			"ETHERNET-"


#define COMMAND_START_CHAR		'U'
#define COMMAND_END_CHAR1		'\r'
#define COMMAND_END_CHAR2		'\n'
#define DOMO_DEVICE_PREFIX1		"ttyUSB"
#define DOMO_DEVICE_PREFIX2		"ttyACM"
#define DOMO_DEVICE_BAUD_RATE		115200
#define DOMO_ID_PREFIX			"DomoSy-"
#define DOMO_ID_MAX_LENGTH		1024
#define DOMO_RESPONSE_START_CHAR	'\t'
#define DOMO_RESPONSE_END_STRING	":)"

/* Messages between client and daemon */
#define MSG_EXIT    		0
#define MSG_GET_DEVICE  	1
#define MSG_CLOSE_DEVICE  	2

#define MSG_SET_PIN_INPUT		10
#define MSG_SET_PIN_OUTPUT		11
#define MSG_SET_PIN_LOW			12
#define MSG_SET_PIN_HIGH		13
#define MSG_GET_PIN_STATUS		14
#define MSG_ANALOG_WRITE		15
#define MSG_ANALOG_READ			16
#define MSG_ANALOG_REFERENCE	17
#define MSG_SET_MASTER 			19
#define MSG_SET_BUTTON			20
#define MSG_SET_PIN_PWM			21
#define MSG_SET_BUT_TOR                 22
#define MSG_REM_BUT_TOR                 23
#define MSG_SET_TOR                     24

#define MSG_READ_DHT11			41
#define MSG_READ_SR04			42

/* Commands between daemon and Domo device */
#define CMD_GET_DEVICE_ID  		0x30

#define CMD_SET_PIN_OUTPUT		0x31
#define CMD_SET_PIN_INPUT		0x34
#define CMD_SET_PIN_HIGH		0x32
#define CMD_SET_PIN_LOW			0x33
#define CMD_GET_PIN_STATUS		0x35

#define CMD_ANALOG_WRITE		0x36
#define CMD_ANALOG_READ			0x37
#define CMD_ANALOG_REFERENCE	        0x38

#define CMD_SET_MASTER			0x39

#define CMD_SET_BUTTON			0x40
#define CMD_SET_BUT_TOR			0x41
#define CMD_REM_BUT_TOR			0x42

#define CMD_READ_DHT11			0x50
#define CMD_READ_SR04			0x51

#define CMD_SET_TOR                     0x60

/*#define ASSERT_TRUE(x) \
    do { \
        if (!(x)) { \
            fprintf(stderr, "%s:%d: ", __func__, __LINE__); \
            perror(#x); \
            exit(EXIT_FAILURE); \
        } \
    } while (0) \
*/
struct DeviceOpen
{
	int fd;			/* file describer for serial device */
	char *devName;	/* serial device name */
	char *id;		/* Domo device id */
	struct DeviceOpen *prev; // List previous
	struct DeviceOpen *next; // List next
};

typedef struct DeviceOpen DeviceOpen;

struct Cli
{
  int id;
  char messageQueueName[MAX_QUEUE_NAME_LENGTH];
  mqd_t mq;
};
typedef struct Cli Cli;
#endif /* #ifndef DOMO_DAEMON_H_ */

