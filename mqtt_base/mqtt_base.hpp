/* This file is part of {{MQTTproject}}, simple MQTT implementation.
 *
 * It's purpose is to demonstrate what we have learned on Network Programming 
 * classes. 
 *
 * This file contains basic structures and constants for use with MQTTBroker and MQTTClient.
 */
#ifndef _MQTT_BASE_H
#define _MQTT_BASE_H
#include <sys/types.h>	/* basic system data types */
#include <sys/socket.h> /* basic socket definitions */
#include <netinet/in.h> /* sockaddr_in{} and other Internet defns */
#include <arpa/inet.h>	/* inet(3) functions */
#include <errno.h>
#include <stdlib.h>
#include <netinet/sctp.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

// CLIENT TYPES
const int PUBLISHER = 0;
const int SUBSCRIBER = 1;

// MSG TYPES
const int INIT = 11;
const int DATA = 12;

// BROKER DEFAULT PORT
const int BROKER_PORT = 7733;
const int BROKER_PORT2 = 7734;

#define SA struct sockaddr
#define LISTENQ 2
#define BUFFSIZE 2048

// COMMON MESSAGE
struct mqtt_msg
{
	int cli_type;	  // constant int SUBSCRIBER or PUBLISHER.
	int msg_type;	  // constant int INIT or DATA
	char topic[20];	  // name of subscribed topic.
	size_t topic_len; // size of topic.
	char data[100];	  // array containing message.
	size_t data_len;  // length of message.
};

// sctp utilities
char *sock_ntop(const struct sockaddr *sa, socklen_t salen);
sctp_assoc_t sctp_address_to_associd(int sock_fd, struct sockaddr *sa, socklen_t salen, sctp_assoc_t assoc_id);
int sctp_get_no_strms(int sock_fd, sctp_assoc_t assoc_id);
int sctp_get_no_strms2(int sock_fd, sctp_assoc_t assoc_id);
void print_notification(char *notify_buf);


class MQTTBroker
{
protected:
	int sock_fd, service, af_family, msg_flags;
	struct sockaddr_in local_addr, remote_addr;
	struct sctp_event_subscribe evnts;
	struct mqtt_msg msg;
	struct sctp_sndrcvinfo sri;
	char readbuf[BUFFSIZE];
	size_t rd_sz;
	std::unordered_map<std::string, std::vector<std::vector<struct sctp_sndrcvinfo>>> topics;
	std::unordered_map<sctp_assoc_t, std::vector<std::string>> sub_assocs;

public:
	// Default constructor
	MQTTBroker();

	// Parametrized constructor
	MQTTBroker(int _service, int _af_family);

protected:
	int prepare_server();
	int set_options();
	int listen_msg();
	int add_to_sub_assocs(struct mqtt_msg *msg_tmp, struct sctp_sndrcvinfo *sri_tmp);
	int add_to_topics(struct mqtt_msg * msg_tmp);
	int notify_subscribers(struct mqtt_msg *msg_tmp, size_t msg_len);
	void print_notification(char *notify_buf);
	int remove_subs(sctp_assoc_t assoc_id);
	int recv_mqtt();
	int send_mqtt(struct mqtt_msg *msg_tmp, size_t msg_len, struct sctp_sndrcvinfo *sri_tmp);

public:
	int start_processing();
};

class MQTTClient
{
protected:
	int sock_fd, service, af_family, addr_len;
	struct sockaddr_in local_addr, broker_addr;
	struct sctp_sndrcvinfo sri;
	struct sctp_event_subscribe evnts;
	struct mqtt_msg msg;
	char readbuf[BUFFSIZE];
	size_t rd_sz;
	char broker_ip[100];
	void (*callback)(struct mqtt_msg*);
	std::unordered_map< std::string, std::function<void(struct mqtt_msg*)>> callbacks;

public:
	// Default constructor
	MQTTClient();

	// Parametrized constructor
	MQTTClient(char *_broker_ip, size_t ip_len, int _service, int _af_family);

protected:
	int prepare_client();
	int set_options();
	int add_callback(std::string topic_name, void (*callback)(struct mqtt_msg*) );
	int run_callback(struct mqtt_msg* msg);

public:
	int recv_mqtt();
	// publisher
	int publish(char *topic, size_t topic_len, char *data, size_t data_len);
	// subscriber
	int subscribe(char *topic, size_t topic_len, void (*callback)(struct mqtt_msg*));
	int listen();
};

#endif
