#include <iostream>
#include "mqtt_base.hpp"
using namespace std;

int main()
{
    char ip_addr[] = "192.168.1.48";
    MQTTClient mqtt_client(ip_addr, sizeof(ip_addr), 7733, AF_INET);
    mqtt_client.send_mqtt();

    return 0;
}
