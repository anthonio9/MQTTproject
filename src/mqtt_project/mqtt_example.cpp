#include <iostream>
#include "mqtt_base.hpp"
using namespace std;

int main()
{
	MQTTBroker mqtt_broker(7733, AF_INET);
	mqtt_broker.start_processing();
	return 0;
}
