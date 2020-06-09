#include "mqtt_base.hpp"
#include <string.h>


using namespace std;

// Default constructor
MQTTBroker::MQTTBroker()
{
	this->service = BROKER_PORT;
	this->af_family = AF_INET;

	if (prepare_server() == 1)
	{
		fprintf(stderr, "prepare_server error!\n");
		exit(1);
	}
}

// Parametrized constructor
MQTTBroker::MQTTBroker(int _service, int _af_family)
{
	this->service = _service;
	this->af_family = _af_family;

	if (prepare_server() == 1)
	{
		fprintf(stderr, "prepare_server error!\n");
		exit(1);
	}
}

int MQTTBroker::getService()
{
	return this->service;
}

int MQTTBroker::prepare_server()
{
	if ((sock_fd = socket(af_family, SOCK_SEQPACKET, IPPROTO_SCTP)) == -1)
	{
		fprintf(stderr, "socket error : %s\n", strerror(errno));
		return 1;
	}

	// Prepare local address structure
	bzero(&local_addr, sizeof(local_addr));
	local_addr.sin_family = af_family;
	local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	local_addr.sin_port = htons(service);

	if (bind(sock_fd, (SA *)&local_addr, sizeof(local_addr)) == -1)
	{
		fprintf(stderr, "bind error : %s\n", strerror(errno));
		return 1;
	}

	if (set_options() == 1)
	{
		fprintf(stderr, "set_options error!\n");
		exit(1);
	}

	if (listen_msg() == 1)
	{
		fprintf(stderr, "listen error!\n");
		exit(1);
	}

	printf("Server prepared!\n");
	return 0; 
}

int MQTTBroker::set_options()
{
	bzero(&evnts, sizeof(evnts));
	evnts.sctp_data_io_event = 1;
	if (setsockopt(sock_fd, IPPROTO_SCTP, SCTP_EVENTS,
				   &evnts, sizeof(evnts)) == -1)
	{
		fprintf(stderr, "setsockopt error : %s\n", strerror(errno));
		return 1;
	}

	printf("Options set!\n");
	return 0;
}

int MQTTBroker::listen_msg()
{
	if (listen(sock_fd, LISTENQ) == -1)
	{
		fprintf(stderr, "listen error : %s\n", strerror(errno));
		return 1;
	}

	return 0;
}

int MQTTBroker::add_to_topics(struct mqtt_msg* msg, struct sctp_sndrcvinfo *sri)
{
	if ( topics.find(msg->topic) == topics.end())
	{
		topics[msg->topic] = vector<vector<struct sctp_sndrcvinfo> > (2); // Initialize  
		topics[msg->topic][SUBSCRIBER] = vector<struct sctp_sndrcvinfo> (); 
		topics[msg->topic][PUBLISHER] = vector<struct sctp_sndrcvinfo> (); 
		printf("New entries for topic: %s:\n", msg->topic.c_str());
	}

	topics[msg->topic][msg->cli_type].push_back(*sri);
	return 0;
}

int MQTTBroker::recv_mqtt()
{
	struct mqtt_msg msg;
	struct sctp_sndrcvinfo sri;

	if (sctp_recvmsg(sock_fd, &msg, sizeof(struct mqtt_msg), NULL, NULL,
				&sri, NULL) == -1) 
	{
		fprintf(stderr,"sctp_recvmsg error : %s\n", strerror(errno));
		return -1;
	}

	if (msg.msg_type == INIT)
	{
		add_to_topics(&msg, &sri);
	}
	return 0;
}

int MQTTBroker::send_mqtt()
{
	return 0;
}

int MQTTBroker::start_processing()
{
	printf("MQTTBroker is running on port: %d.\n", this->service);
	while(true){
		recv_mqtt();
	}
	
	return 0;
}

