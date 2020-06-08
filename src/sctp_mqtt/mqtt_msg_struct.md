# Basic message used for communication between clients and broker

```cpp
struct mqtt_msg {
	char[10] topic; 		// name of subscribed topic.
	size_t topic_len; 		// size of topic.
	int cli_type;			// constant int SUBSCRIBER or PUBLISHER.
	size_t cli_type_len;	// size of cli_type.
	char[10] data;			// array containing message.
	size_t data_len;		// length of message.
};

```
