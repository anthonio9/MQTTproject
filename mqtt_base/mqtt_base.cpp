#include "mqtt_base.hpp"
#include <string.h>

using namespace std;

char * sock_ntop(const struct sockaddr *sa, socklen_t salen)
{
    char		portstr[8];
    static char str[128];		/* Unix domain is largest */

	switch (sa->sa_family) {
	case AF_INET: {
		struct sockaddr_in	*sin = (struct sockaddr_in *) sa;

		if (inet_ntop(AF_INET, &sin->sin_addr, str, sizeof(str)) == NULL)
			return(NULL);
		if (ntohs(sin->sin_port) != 0) {
			snprintf(portstr, sizeof(portstr), ":%d", ntohs(sin->sin_port));
			strcat(str, portstr);
		}
		return(str);
	}
/* end sock_ntop */

	case AF_INET6: {
		struct sockaddr_in6	*sin6 = (struct sockaddr_in6 *) sa;

		str[0] = '[';
		if (inet_ntop(AF_INET6, &sin6->sin6_addr, str + 1, sizeof(str) - 1) == NULL)
			return(NULL);
		if (ntohs(sin6->sin6_port) != 0) {
			snprintf(portstr, sizeof(portstr), "]:%d", ntohs(sin6->sin6_port));
			strcat(str, portstr);
			return(str);
		}
		return (str + 1);
	}

	default:
		snprintf(str, sizeof(str), "sock_ntop: unknown AF_xxx: %d, len %d",
				 sa->sa_family, salen);
		return(str);
	}
    return (NULL);
}


sctp_assoc_t sctp_address_to_associd(int sock_fd, struct sockaddr *sa, socklen_t salen, sctp_assoc_t assoc_id)
{
	struct sctp_paddrparams sp;
	socklen_t siz;
/*
struct sctp_paddrparams {
    sctp_assoc_t               spp_assoc_id;
    struct sockaddr_storage    spp_address;
    uint32_t                   spp_hbinterval;
    uint16_t                   spp_pathmaxrxt;
};
*/
		
	siz = sizeof(struct sctp_paddrparams);
	bzero(&sp,siz);
	memcpy(&sp.spp_address,sa,salen);
	sctp_opt_info(sock_fd,0,
		   SCTP_PEER_ADDR_PARAMS, &sp, &siz);
	return(sp.spp_assoc_id);
}


int sctp_get_no_strms(int sock_fd, sctp_assoc_t assoc_id)
{
	socklen_t retsz;
	struct sctp_status status;
/* struct sctp_status {
        sctp_assoc_t            sstat_assoc_id;
        __s32                   sstat_state;
        __u32                   sstat_rwnd;
        __u16                   sstat_unackdata;
        __u16                   sstat_penddata;
        __u16                   sstat_instrms;
        __u16                   sstat_outstrms;
        __u32                   sstat_fragmentation_point;
        struct sctp_paddrinfo   sstat_primary;
};*/	
	retsz = sizeof(status);	
	bzero(&status,sizeof(status));

	status.sstat_assoc_id = assoc_id;
	if( sctp_opt_info(sock_fd,assoc_id, SCTP_STATUS,
		   &status, &retsz) <0 ){
		perror("sctp_get_no_strms:sctp_opt_info error");
		exit(-1);
	}

	return(status.sstat_outstrms);
}

int sctp_get_no_strms2(int sock_fd, sctp_assoc_t assoc_id)
{
	socklen_t retsz;
	struct sctp_status status;
	retsz = sizeof(status);	
	bzero(&status,sizeof(status));

	status.sstat_assoc_id = assoc_id;
	if( getsockopt(sock_fd,IPPROTO_SCTP, SCTP_STATUS,
		   &status, &retsz) <0 ){
		perror("sctp_get_no_strms:getsockopt error");
		exit(-1);
	}
	return(status.sstat_outstrms);
}


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
	evnts.sctp_shutdown_event = 1;
	if (setsockopt(sock_fd, IPPROTO_SCTP, SCTP_EVENTS, &evnts, sizeof(evnts)) < 0)
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

int MQTTBroker::add_to_topics(struct mqtt_msg * msg_tmp)
{
	//char topic_buff[msg.topic_len];
	//strncpy(topic_buff, msg.topic, msg.topic_len);
	//string topic_tmp(topic_buff);
	string topic_tmp(msg_tmp->topic);

	if (topics.empty() || topics.find(topic_tmp) == topics.end())
	{
		topics[topic_tmp] = vector<vector<struct sctp_sndrcvinfo> > (2); // Initialize  
		topics[topic_tmp][SUBSCRIBER] = vector<struct sctp_sndrcvinfo> (); 
		topics[topic_tmp][PUBLISHER] = vector<struct sctp_sndrcvinfo> (); 
		printf("New topic: %s.\n", topic_tmp.c_str());
	}

	topics[topic_tmp][msg_tmp->cli_type].push_back(sri);

	if (msg_tmp->cli_type == SUBSCRIBER)
		printf("New topic entry: SUBSCRIBER\n");
	else
		printf("New topic entry: PUBLISHER\n");
	return 0;
}

int MQTTBroker::add_to_sub_assocs(struct mqtt_msg *msg_tmp, struct sctp_sndrcvinfo *sri_tmp)
{
	string topic_tmp(msg_tmp->topic);

	if (sub_assocs.empty() || sub_assocs.find(sri_tmp->sinfo_assoc_id) == sub_assocs.end())
		sub_assocs[sri_tmp->sinfo_assoc_id] = vector<string>(); // Initialize  

	sub_assocs[sri_tmp->sinfo_assoc_id].push_back(topic_tmp); // Add topic to subscriber
	printf("Assoc_id: %d, added topic: %s\n", sri_tmp->sinfo_assoc_id, topic_tmp.c_str());

	return 0;
}
//int MQTTBroker::notify_subscribers()
//{
//	printf("Notifying subscribers on topic: %s\n", msg.topic);
//	string topic_tmp(msg.topic);
//
//	if (topics.empty() || topics.find(topic_tmp) == topics.end()){
//		fprintf(stderr, "error: notify_subscribers, no topic: %s!!!\n", topic_tmp.c_str());
//		return 1;
//	}
//
//	for(struct sctp_sndrcvinfo sri_tmp : topics[topic_tmp][SUBSCRIBER])
//	{
//		send_mqtt(&msg, sizeof(msg), &sri_tmp);
//		printf("Subscriber on topic: %s notified!\n", topic_tmp.c_str());
//	}
//
//	return 0;
//}
//
int MQTTBroker::notify_subscribers(struct mqtt_msg *msg_tmp, size_t msg_len)
{
	printf("Notifying subscribers on topic: %s\n", msg_tmp->topic);
	string topic_tmp(msg_tmp->topic);

	if (topics.empty() || topics.find(topic_tmp) == topics.end()){
		fprintf(stderr, "error: notify_subscribers, no topic: %s!!!\n", topic_tmp.c_str());
		return 1;
	}

	for(struct sctp_sndrcvinfo sri_tmp : topics[topic_tmp][SUBSCRIBER])
	{
		send_mqtt(msg_tmp, msg_len, &sri_tmp);
		printf("Subscriber on topic: %s notified!\n", topic_tmp.c_str());
	}

	return 0;
}

int MQTTBroker::remove_subs(sctp_assoc_t assoc_id)
{
	if (sub_assocs.empty() || sub_assocs.find(assoc_id) == sub_assocs.end())
	{
		fprintf(stderr, "error: no assoc_id: %d!\n", assoc_id);
		return 1;
	}

	struct sctp_sndrcvinfo *sri_tmp;

	for( string topic_tmp : sub_assocs[assoc_id])
	{
		if (topics.empty() || topics.find(topic_tmp) == topics.end()){
			fprintf(stderr, "error: remove_subs, no topic: %s!!!\n", topic_tmp.c_str());
			return 1;
		}

		// Search for particular assoc_id in vector of struct sctp_sndrcvinfo;
		for(size_t i = 0; i < topics[topic_tmp][SUBSCRIBER].size(); ++i)
		{
			sri_tmp = &topics[topic_tmp][SUBSCRIBER][i];
			
			if(sri_tmp->sinfo_assoc_id == assoc_id)
			{
				topics[topic_tmp][SUBSCRIBER].erase(topics[topic_tmp][SUBSCRIBER].begin() + i);
				printf("Assoc_id: %d, topic removed: %s\n", assoc_id, topic_tmp.c_str());
				break;
			}
		}
	}

	sub_assocs.erase(assoc_id);
	printf("Assoc_id: %d, removed from sub_assocs.\n", assoc_id);

	return 0;
}

void MQTTBroker::print_notification(char *notify_buf)
{
	union sctp_notification *snp;
	struct sctp_assoc_change *sac;
	struct sctp_paddr_change *spc;
	struct sctp_remote_error *sre;
	struct sctp_send_failed *ssf;
	struct sctp_shutdown_event *sse;
	struct sctp_adaptation_event *ae;
	struct sctp_pdapi_event *pdapi;
	const char *str;

	snp = (union sctp_notification *)notify_buf;
	switch(snp->sn_header.sn_type) {
	case SCTP_ASSOC_CHANGE:
		sac = &snp->sn_assoc_change;
		switch(sac->sac_state) {
		case SCTP_COMM_UP:
			str = "COMMUNICATION UP";
			break;
		case SCTP_COMM_LOST:
			str = "COMMUNICATION LOST";
			break;
		case SCTP_RESTART:
			str = "RESTART";
			break;
		case SCTP_SHUTDOWN_COMP:
			str = "SHUTDOWN COMPLETE";
			break;
		case SCTP_CANT_STR_ASSOC:
			str = "CAN'T START ASSOC";
			break;
		default:
			str = "UNKNOWN";
			break;
		} /* end switch(sac->sac_state) */
		printf("SCTP_ASSOC_CHANGE: %s, assoc=0x%x\n", str,
		       (uint32_t)sac->sac_assoc_id);
		break;
	case SCTP_PEER_ADDR_CHANGE:
	/*
	enum sctp_spc_state {
        SCTP_ADDR_AVAILABLE,
        SCTP_ADDR_UNREACHABLE,
        SCTP_ADDR_REMOVED,
        SCTP_ADDR_ADDED,
        SCTP_ADDR_MADE_PRIM,
        SCTP_ADDR_CONFIRMED,
};

	*/
		spc = &snp->sn_paddr_change;
		switch(spc->spc_state) {
		case SCTP_ADDR_AVAILABLE:
			str = "ADDRESS AVAILABLE";
			break;
		case SCTP_ADDR_UNREACHABLE:
			str = "ADDRESS UNREACHABLE";
			break;
		case SCTP_ADDR_REMOVED:
			str = "ADDRESS REMOVED";
			break;
		case SCTP_ADDR_ADDED:
			str = "ADDRESS ADDED";
			break;
		case SCTP_ADDR_MADE_PRIM:
			str = "ADDRESS MADE PRIMARY";
			break;
		case SCTP_ADDR_CONFIRMED:
			str = "SCTP ADDR CONFIRMED";
			break;
		default:
			str = "UNKNOWN";
			break;
		} /* end switch(spc->spc_state) */
		printf("SCTP_PEER_ADDR_CHANGE: %s, addr=%s, assoc=0x%x\n", str,
		       sock_ntop((SA *)&spc->spc_aaddr, sizeof(spc->spc_aaddr)),
		       (uint32_t)spc->spc_assoc_id);
		break;
	case SCTP_REMOTE_ERROR:
		sre = &snp->sn_remote_error;
		printf("SCTP_REMOTE_ERROR: assoc=0x%x error=%d\n",
		       (uint32_t)sre->sre_assoc_id, sre->sre_error);
		break;
	case SCTP_SEND_FAILED:
		ssf = &snp->sn_send_failed;
		printf("SCTP_SEND_FAILED: assoc=0x%x error=%d\n",
		       (uint32_t)ssf->ssf_assoc_id, ssf->ssf_error);
		break;
	case SCTP_ADAPTATION_INDICATION:
		ae = &snp->sn_adaptation_event;
		printf("SCTP_ADAPTATION_INDICATION: 0x%x\n",
		    (u_int)ae->sai_adaptation_ind);
		break;
	case SCTP_PARTIAL_DELIVERY_EVENT:
	    pdapi = &snp->sn_pdapi_event;
	    if(pdapi->pdapi_indication == SCTP_PARTIAL_DELIVERY_ABORTED)
		    printf("SCTP_PARTIAL_DELIEVERY_ABORTED\n");
	    else
		    printf("Unknown SCTP_PARTIAL_DELIVERY_EVENT 0x%x\n",
			   pdapi->pdapi_indication);
	    break;
	case SCTP_SHUTDOWN_EVENT:
		sse = &snp->sn_shutdown_event;
		remove_subs(sse->sse_assoc_id);
		printf("SCTP_SHUTDOWN_EVENT: assoc=0x%x\n",
		       (uint32_t)sse->sse_assoc_id);
		break;
	default:
		printf("Unknown notification event type=0x%x\n", 
		       snp->sn_header.sn_type);
	}
}

int MQTTBroker::recv_mqtt()
{
	if (sctp_recvmsg(sock_fd, readbuf, sizeof(readbuf), NULL, NULL,
					 &sri, &msg_flags) == -1)
	{
		fprintf(stderr, "sctp_recvmsg error : %s\n", strerror(errno));
		return -1;
	}

	if(msg_flags & MSG_NOTIFICATION) {
		printf("SCTP notification received!\n");
		print_notification(readbuf);
	}

	//convert readbuf to mqtt_msg;
	memcpy(&msg, readbuf, sizeof(msg));

	if (msg.msg_type == INIT)
	{
		printf("Received init request.\n");
		add_to_topics(&msg);
		add_to_sub_assocs(&msg, &sri);
	}

	if (msg.msg_type == DATA && msg.cli_type == PUBLISHER)
	{
		printf("Received data request on topic: %s\n", msg.topic);
		notify_subscribers(&msg, sizeof(msg));

	}

	return 0;
}

int MQTTBroker::send_mqtt(struct mqtt_msg *msg_tmp, size_t msg_len, struct sctp_sndrcvinfo *sri_tmp)
{
	if ((sctp_send(sock_fd, msg_tmp, msg_len, sri_tmp, 0)) < 0)
	{
		fprintf(stderr, "sctp_sendmsg : %s\n", strerror(errno));
		return 1;
	}
	return 0;
}

int MQTTBroker::start_processing()
{
	printf("MQTTBroker is running on port: %d.\n", this->service);
	printf("Waiting for clients!\n");
	while (true)
	{
		recv_mqtt();
	}

	return 0;
}

MQTTClient::MQTTClient()
{
	this->service = BROKER_PORT;
	this->af_family = AF_INET;
	char localhost[] = "localhost";
	strncpy(this->broker_ip, localhost, strlen(localhost));

	if (prepare_client() == 1)
	{
		fprintf(stderr, "prepare_client error!\n");
		exit(1);
	}

	if (set_options() == 1)
	{
		fprintf(stderr, "set_options error!\n");
		exit(1);
	}
}

// Parametrized constructor
MQTTClient::MQTTClient(char *_broker_ip, size_t ip_len, int _service, int _af_family)
{
	strncpy(this->broker_ip, _broker_ip, ip_len);
	this->service = _service;
	this->af_family = _af_family;

	printf("broker_ip: %s\n", this->broker_ip);

	if (prepare_client() == 1)
	{
		fprintf(stderr, "prepare_client error!\n");
		exit(1);
	}

	if (set_options() == 1)
	{
		fprintf(stderr, "set_options error!\n");
		exit(1);
	}
}

int MQTTClient::prepare_client()
{
	bzero(&broker_addr, sizeof(broker_addr));

	if (inet_pton(af_family, this->broker_ip, &broker_addr.sin_addr) < 0)
	{
		fprintf(stderr, "inet_pton error for %s : %s \n", broker_ip, strerror(errno));
		return 1;
	}

	broker_addr.sin_family = af_family;
	broker_addr.sin_port = htons(this->service);
	addr_len = sizeof(broker_addr);

	if ((sock_fd = socket(af_family, SOCK_SEQPACKET, IPPROTO_SCTP)) <= 0)
	{
		fprintf(stderr, "socket error : %s\n", strerror(errno));
		return 1;
	}

	printf("Client prepared!\n");
	return 0;
}

int MQTTClient::set_options()
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

int MQTTClient::send_mqtt()
{
	char topic[] = "topic1";
	// char data[] = "Wiadomość do Antoniego od Natalii";

	msg.cli_type = PUBLISHER;
	msg.msg_type = INIT;
	strncpy(msg.topic, topic, sizeof(topic));
	msg.topic_len = sizeof(msg.topic);
	//strncpy(msg.data, data, sizeof(data));
	//msg.data_len = sizeof(msg.data);

	if (sendto(sock_fd, &msg, sizeof(msg), 0, (SA *)&broker_addr, sizeof(broker_addr)) == -1)
	{
		fprintf(stderr, "sendto error : %s\n", strerror(errno));
		return 1;
	}

	printf("msg: %s, send to: %s\n", msg.topic, this->broker_ip);
	return 0;
}

int MQTTClient::recv_mqtt()
{
	if (recv(sock_fd, &msg, sizeof(msg), 0) == -1)
	{
		fprintf(stderr, "sctp_recvmsg error : %s\n", strerror(errno));
		return 1;
	}

	//printf("received mqtt_msg:\nmsg.cli_type: %d\nmsg.msg_type: %d\nmsg.topic: %s\nmsg.topic_len: %ld\nmsg.data: %s\nmsg.data_len: %ld\n\n", 
	//			msg.cli_type, msg.msg_type, msg.topic, msg.topic_len, msg.data, msg.data_len);
	return 0;
}

int MQTTClient::publish(char *topic, size_t topic_len, char *data, size_t data_len)
{
	msg.cli_type = PUBLISHER;
	msg.msg_type = DATA;
	strncpy(msg.topic, topic, topic_len);
	msg.topic_len = sizeof(msg.topic);
	strncpy(msg.data, data, data_len);
	msg.data_len = sizeof(msg.data);

	if (sendto(sock_fd, &msg, sizeof(msg), 0, (SA *)&broker_addr, sizeof(broker_addr)) == -1)
	{
		fprintf(stderr, "sendto error : %s\n", strerror(errno));
		return 1;
	}

	printf("published data: %s to topic: %s\n", msg.data, msg.topic);

	return 0;
}

int MQTTClient::add_callback(string topic_name, void (*callback)(struct mqtt_msg*) )
{
	if (callbacks.find(topic_name) != callbacks.end()){
		fprintf(stderr, "error: add_callback, topic already exists: %s!!!\n", topic_name.c_str());
		return 1;
	}

	callbacks[topic_name] = callback;
	printf("add_callback: callback function added to topic: %s.\n", topic_name.c_str());
	return 0;
}

int MQTTClient::run_callback(struct mqtt_msg *msg)
{
	string topic_tmp(msg->topic);
	if (callbacks.empty() || callbacks.find(topic_tmp) == callbacks.end())
	{
		fprintf(stderr,"run_callback: no topic in stored calbacks: %s.\n", msg->topic);
		return 1;
	}

	callbacks[topic_tmp](msg);
	return 0;
}

int MQTTClient::subscribe(char *topic, size_t topic_len, void (*callback_)(struct mqtt_msg*) )
{
	this->callback = callback_;
	msg.cli_type = SUBSCRIBER;
	msg.msg_type = INIT;
	strncpy(msg.topic, topic, topic_len);
	msg.topic_len = sizeof(msg.topic);
	//strncpy(msg.data, data, sizeof(data));
	//msg.data_len = sizeof(msg.data);

	if (sendto(sock_fd, &msg, sizeof(msg), 0, (SA *)&broker_addr, sizeof(broker_addr)) == -1)
	{
		fprintf(stderr, "sendto error : %s\n", strerror(errno));
		return 1;
	}

	string topic_name(topic);
	add_callback(topic_name, callback);

	printf("subscribed to topic: %s\n", msg.topic);

	return 0;
}

int MQTTClient::listen()
{
	while(true)
	{
		bzero(&msg, sizeof(struct mqtt_msg));
		if (recv_mqtt() != 0){
			printf("recv_mqtt error");
			continue;
		}

		run_callback(&msg);
		//(*this->callback)(&msg);
	}

	return 0;
}

int MQTTClient::unsubscribe()
{
	return 0;
}
