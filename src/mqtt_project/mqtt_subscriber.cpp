#include <iostream>
#include "mqtt_base.hpp"
using namespace std;

void callback(struct mqtt_msg *msg){
	printf("Data: %s\n", msg->data);
}

void callback1(int x){
	printf("Received data: %d\n", x);
}


int main()
{
    char ip_addr[] = "127.0.0.1";
    char topic[] = "temat Natalii";
    char data[] = "Hej Antos";
    MQTTClient mqtt_client(ip_addr, sizeof(ip_addr), 7733, AF_INET);
    mqtt_client.subscribe(topic, sizeof(topic), &callback);
	mqtt_client.listen();
    return 0;
}
