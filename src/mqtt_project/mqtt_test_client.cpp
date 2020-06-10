#include <iostream>
#include "mqtt_base.hpp"
#include <string.h>
#include <sys/types.h>	/* basic system data types */
#include <sys/socket.h> /* basic socket definitions */
#include <netinet/in.h> /* sockaddr_in{} and other Internet defns */
#include <arpa/inet.h>	/* inet(3) functions */
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/sctp.h>
#include <unistd.h>
using namespace std;

#define SA struct sockaddr
#define MAXLINE 10
#define MAXLINE2 128
#define SERV_PORT 7733
#define SCTP_MAXLINE 128
#define SERV_MAX_SCTP_STRM 10	/* normal maximum streams */
#define SERV_MORE_STRMS_SCTP 20 /* larger number of streams */

int main()
{
	int sock_fd;
	struct sockaddr_in servaddr;
	struct sctp_event_subscribe evnts;
	struct mqtt_msg msg;

	if ((sock_fd = socket(AF_INET, SOCK_SEQPACKET, IPPROTO_SCTP)) == -1)
	{
		fprintf(stderr, "socket error : %s\n", strerror(errno));
		return 1;
	}

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);
	if (inet_pton(AF_INET, "192.168.1.48", &servaddr.sin_addr) < 1)
	{
		fprintf(stderr, "inet_pton error \n");
		return 1;
	}

	bzero(&evnts, sizeof(evnts));
	evnts.sctp_data_io_event = 1;

	if (setsockopt(sock_fd, IPPROTO_SCTP, SCTP_EVENTS,
				   &evnts, sizeof(evnts)) > 0)
	{
		fprintf(stderr, "setsockopt error : %s\n", strerror(errno));
		return 1;
	}

	char test_buff[] = "test_topic";

	msg.cli_type = SUBSCRIBER;
	msg.msg_type = INIT;
	strncpy(msg.topic, test_buff, sizeof(test_buff));
	msg.topic_len = sizeof(test_buff);

	sendto(sock_fd, &msg, sizeof(msg), 0, (SA *)&servaddr, sizeof(servaddr));
	close(sock_fd);
	return (0);
}
