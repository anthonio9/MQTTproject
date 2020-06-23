# MQTT BASE

**mqtt_base** library contains **MQTTBroker** and **MQTTClient** classes. These are a simple implementation of the MQTT protocol that meets the functionality of *broker*, *publisher* and *subscriber*.
Communiation between modules is based on SCTP one-to-many. To further investigate the library go to [**mqtt_base**](mqtt_base) directory.

## Developers

* Antoni Jankowski
* Natalia Skawińska

## MQTTBroker

MQTTBroker implements an instance of *broker* - process broadcasting messages published by *publishers* to all clients subscribing given topic.
By default broker process runs on port **7733**, both on IPv4 and IPv6, however this can be modified whilst using a parametrised constructor.

* topics - dict with lists of publishers and subscribers bound to certain topic.
* sub_assocs - dict/unordered_map associating association id with subscribed topics.

### Public
+ MQTTBroker()
	default constructor,
+ MQTTBroker(int \_service, int \_af_family)
	parametrised constructor, create instance, af_family, local_port
+ start_processing()
	Executes the process of handling clients and their messages.

### Protected
+ prepare\_server() 
	Create server socket and bind it to port and address.
+ set\_options()
	Sets socket options.
+ listen_msg()
	Puts socket in listening state.
+ add_to_sub_assocs(struct mqtt_msg \*msg_tmp, struct sctp_sndrcvinfo \*sri_tmp) 
	Manages addition of associations of the new subscribers to **sub\_assocs** unordered\_map.
+ add\_to\_topics(struct mqtt_msg \*msg_tmp)
	Manages adding new clients to **topic** unordered_map.
+ notify_subscribers(struct mqtt_msg \*msg_tmp, size_t msg_len) 
	Send message to subscribers subscribed to given topic.
+ print_notification(char \*notify_buf)
+ remove_subs(sctp_assoc_t assoc_id)
	Removes subscriber of given association id from **topics** and **sub_assocs** unordered_maps.
+ recv_mqtt() 
	Receives MQTT/SCTP message and manages it handling.
+ send_mqtt()
	Send given message to a client.


## MQTTClient

MQTTClient implements functionality of subscriber and publisher. One can publish and subsribe using one class instance.
This class is also capable of subscribing multiple topics with a callback to each of them.

### Public

+ MQTTClient()
	Default constructor.
+ MQTTClient(char \_broker_ip,int \_service, int \_af_family) 
	Parametrised constructor.
+ recv_mqtt()
	Receives mqtt message.
+ publish(char \*topic, size_t topic_len, char \*data, size_t data_len)
	Sends mqtt_msg to given topic and with given data to MQTTBroker.
+ subscribe(char \*topic, size_t topic_len, void (\*callback)(struct mqtt_msg\*)) 
	Subscribes given topic.
+ listen()
	start listening for messages on subscribed topics

**Private:**

- prepare_client() 
	Prepare client for connecting the MQTTBroker.
- set_options()
	Sets socket options.
+ add_callback(std::string topic_name, void (\*callback)(struct mqtt_msg\*))
	Adds callback to given topic.
+ run_callback(struct mqtt_msg\* msg))
	Runs callback assigned to given message and topic.

