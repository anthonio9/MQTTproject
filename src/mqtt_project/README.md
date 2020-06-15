# Simple MQTT BASE library

Biblioteka mqtt_base zawiera klasy serwera i klienta, które są prostą implementacji protokołu MQTT spełniającego funkcjonalności protokołu : **broker**, **publisher**, **subscriber**.

## MQTTBroker

- topics - dict with lists of publishers and subscribers bound to certain topic.

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

**Public**

- MQTTClient() - create instance, af_family, srv_address, srv_port
- MQTTClient(char _\_broker_ip,int \_service, int \_af_family) - parametrised constructor, create instance, af_family, local_port_ recv_mqtt() - wait for mqtt message.
- send_mqtt() - send mqtt message.
- recv_mqtt() - wait for mqtt message.
- publish(char *topic, size_t topic_len, char *data, size_t data_len) - publish message on certain topic
- subscribe(char \*topic, size_t topic_len) - subscribe to certain topic
- listen() - start listening for messages on subscribed topics
- unsubscribe() - unsubscribe from certain topic

**Private:**

- prepare_client() - create client socket.
- set_options() - set socket options.
