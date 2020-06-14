#include <iostream>
#include "mqtt_base.hpp"
using namespace std;

int main()
{
    char ip_addr[] = "127.0.0.1";
    char topic[] = "temat Natalii";
    char data[] = "Hej Antos";
    MQTTClient mqtt_client(ip_addr, sizeof(ip_addr), 7733, AF_INET);
    mqtt_client.publish(topic, sizeof(topic), data, sizeof(data));
    return 0;
}
