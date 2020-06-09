/* * This file is part of {{MQTTproject}}, simple MQTT implementation.  *
 * It's purpose is to demonstrate what we have learned on Network Programming 
 * classes. 
 *
 * This file contains basic structures and constants for use with ServerMQTT and ClientMQTT.
 */
#include <iostream>
#ifndef MQTT_BASE_H
#define MQTT_BASE_H

// CLIENT TYPES
const int PUBLISHER = 1;
const int SUBSCRIBER = 2;

// MSG TYPES
const int INIT = 11;
const int MSG = 12;

// BROKER DEFAULT PORT
const int BROKER_PORT = 7733;
const int BROKER_PORT2 = 7734;

// COMMON MESSAGE
struct mqtt_msg {
	int cli_type;			// constant int SUBSCRIBER or PUBLISHER.
	int msg_type;			// constant int INIT or MSG
	char topic[20]; 		// name of subscribed topic.
	std::size_t topic_len; 		// size of topic.
	char data[100];			// array containing message.
	std::size_t data_len;		// length of message.
};

#endif
