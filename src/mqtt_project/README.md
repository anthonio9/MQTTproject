# Simple MQTT BASE library

Biblioteka mqtt_base zawiera klasy serwera i klienta, które są prostą implementacji protokołu MQTT spełniającego funkcjonalności protokołu : **broker**, **publisher**, **subscriber**.

## MQTTBroker

* topics - dict with lists of publishers and subscribers bound to certain topic.
* MQTTBroker() - default constructor,
* MQTTBroker(int \_service) - parametrised constructor, create instance, af_family, local_port
* getService() - return service port number.
* prepare_server()
* set_options()
* listen_msg()

## MQTTClient

* MQTTClient() - create instance, af_family, srv_address, srv_port


