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

int MQTTBroker::add_to_topics()
{
	//char topic_buff[msg.topic_len];
	//strncpy(topic_buff, msg.topic, msg.topic_len);
	//string topic_tmp(topic_buff);
	string topic_tmp(msg.topic);

	if (topics.empty() || topics.find(topic_tmp) == topics.end())
	{
		topics[topic_tmp] = vector<vector<struct sctp_sndrcvinfo> > (2); // Initialize  
		topics[topic_tmp][SUBSCRIBER] = vector<struct sctp_sndrcvinfo> (); 
		topics[topic_tmp][PUBLISHER] = vector<struct sctp_sndrcvinfo> (); 
		printf("New entries for topic: %s:\n", topic_tmp.c_str());
	}

	topics[topic_tmp][msg.cli_type].push_back(sri);
	return 0;
}

int MQTTBroker::notify_subscribers()
{
	string topic_tmp(msg.topic);
	if (topics.empty() || topics.find(topic_tmp) == topics.end()){
		fprintf(stderr, "notify_subscribers: no topic: %s!!!\n", topic_tmp.c_str());
		return 1;
	}

	for(struct sctp_sndrcvinfo sri : topics[topic_tmp][PUBLISHER])
	{
		send_mqtt(&sri, &msg, sizeof(msg));
	}
}

int MQTTBroker::recv_mqtt()
{
	if (sctp_recvmsg(sock_fd, &msg, sizeof(struct mqtt_msg), NULL, NULL,
				&sri, NULL) == -1) 
	{
		fprintf(stderr,"sctp_recvmsg error : %s\n", strerror(errno));
		return -1;
	}

	if (msg.msg_type == INIT)
	{
		printf("Received init request.\n");
		add_to_topics();
	}

	if (msg.msg_type == DATA && msg.cli_type == PUBLISHER)
	{
		printf("Received data request.\n");
		notify_subscribers();
	}

	return 0;
}

int MQTTBroker::send_mqtt(struct sctp_sndrcvinfo* sri, struct mqtt_msg *msg_tmp, size_t msg_tmp_len)
{
	if( (sctp_send(sock_fd, msg_tmp, msg_tmp_len, 
			 &sri, 0)) < 0 ){
			fprintf(stderr,"sctp_sendmsg : %s\n", strerror(errno));
			return 1;
	}
	return 0;
}

int MQTTBroker::start_processing()
{
	printf("MQTTBroker is running on port: %d.\n", this->service);
	printf("Waiting for clients!\n");
	while(true){
		recv_mqtt();
	}
	
	return 0;
}

