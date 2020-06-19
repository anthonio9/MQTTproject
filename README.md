# MQTT BASE

**mqtt_base** library contains **MQTTBroker** and **MQTTClient** classes. These are a simple implementation of the MQTT protocol that meets the functionality of *broker*, *publisher* and *subscriber*.
Communiation between modules is based on SCTP one-to-many. To further investigate the library go to **[mqtt_base](mqtt_base)** directory.

## Developers

* Antoni Jankowski
* Natalia Skawińska

## MQTTBroker

MQTTBroker implements an instance of *broker* - process broadcasting messages published by *publishers* to all clients subscribing given topic.
By default broker process runs on port **7733**, both on IPv4 and IPv6, however this can be modified whilst using a parametrised constructor.

* topics - dict with lists of publishers and subscribers bound to certain topic.

**Public:**
- MQTTBroker() - default constructor,
- MQTTBroker(int \_service, int \_af_family) - parametrised constructor, create instance, af_family, local_port
- getService() - return service port number.
- start_processing() - start MQTTBroker forever loop.

**Private:**
- prepare_server() - create server socket and bind it to port and address.
- set_options() - set socket options.
- listen_msg() - put socket in listening state.
- add_to_topics() - add subscriber to topics.
- notify_subscribers() - send message to subscribers subscribed to certain topic.
- recv_mqtt() - wait for mqtt message.
- send_mqtt() - send mqtt message.


## MQTTClient

MQTTClient implements functionality of subscriber and publisher. One can publish and subsribe using one class instance.
This class is also capable of subscribing multiple topics with a callback to each of them.

**Public**

+ MQTTClient() - create instance, af_family, srv_address, srv_port
+ MQTTClient(char \_broker_ip,int \_service, int \_af_family) - parametrised constructor, create instance, af_family, local_port_ recv_mqtt() - wait for mqtt message.
+ send_mqtt() - send mqtt message.
+ recv_mqtt() - wait for mqtt message.
+ publish(char \*topic, size_t topic_len, char \*data, size_t data_len) - publish message on certain topic
+ subscribe(char \*topic, size_t topic_len) - subscribe to certain topic
+ listen() - start listening for messages on subscribed topics
+ unsubscribe() - unsubscribe from certain topic

**Private:**

- prepare_client() - create client socket.
- set_options() - set socket options.
