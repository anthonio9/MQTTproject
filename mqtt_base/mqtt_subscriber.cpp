#include <iostream>
#include "mqtt_base.hpp"
using namespace std;

void callback1(struct mqtt_msg *msg){
	printf("callback1, received data: %s\n", msg->data);
}

void callback2(struct mqtt_msg *msg){
	printf("callback2, received data: %s\n", msg->data);
}


int main()
{
    char ip_addr[] = "127.0.0.1";
    char topic1[] = "Natalie's topic";
	char topic2[] = "Anthonio's topic"; 

    MQTTClient mqtt_client(ip_addr, sizeof(ip_addr), 7733, AF_INET);
    mqtt_client.subscribe(topic1, sizeof(topic1), &callback1);
	mqtt_client.subscribe(topic2, sizeof(topic2), &callback2);
	mqtt_client.listen();
    return 0;
}
