#include <iostream>
#include "mqtt_base.hpp"
using namespace std;

int main()
{
	MQTTBroker mqtt_broker(7769, AF_INET);
	printf("mqtt_base values: BROKER_PORT: %d\n", BROKER_PORT);
	printf("service mqtt: %d\n", mqtt_broker.getService());
	return 0;
}
