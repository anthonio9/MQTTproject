#include <iostream>
#include "mqtt_base.hpp"
using namespace std;

int main()
{
    char ip_addr[] = "127.0.0.1";
    char topic[] = "tematNatalii";
    char data[] = "Hej Antos";
    MQTTClient mqtt_client(ip_addr, sizeof(ip_addr), 7733, AF_INET);
    mqtt_client.subscribe(topic, sizeof(topic));
    printf("sub\n");
    mqtt_client.recv_mqtt();
    printf("recv\n");
    return 0;

}
