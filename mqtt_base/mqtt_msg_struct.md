# Basic message used for communication between clients and broker

```cpp
struct mqtt_msg {
	int cli_type;			// constant int SUBSCRIBER or PUBLISHER.
	int msg_type;			// constant int INIT or MSG.
	char topic[20]; 		// name of subscribed topic.
	size_t topic_len; 		// size of topic.
	char data[100];			// array containing message.
	size_t data_len;		// length of message.
};

```
