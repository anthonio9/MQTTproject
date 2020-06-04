#include <sys/types.h>   /* basic system data types */
#include <sys/socket.h>  /* basic socket definitions */
#include <netinet/in.h>  /* sockaddr_in{} and other Internet defns */
#include <arpa/inet.h>   /* inet(3) functions */
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/sctp.h>

using namespace std;

#define SA      struct sockaddr
#define LISTENQ 2
#define SERV_PORT 7
#define BUFFSIZE 2048


class BaseSCTP {
	protected:
		int sock_fd, service, af_family;
		struct sockaddr_in local_addr, remote_addr;	
		struct sctp_sndrcvinfo sri;
		struct sctp_event_subscribe evnts;
		char readbuf[BUFFSIZE];
		size_t rd_sz;
		
	// Default constructor
	public: 
	BaseSCTP(){
		service = 7733;
		af_family = AF_INET;

		if ( prepare_socket() == 1){
			fprintf(stderr, "prepare_socket error!\n");
			exit(1);
		}
	}

	// Parametrized constructor
	BaseSCTP(int _service, int _af_family){
		service = _service;
		af_family = _af_family;

		if ( prepare_socket() == 1){
			fprintf(stderr, "prepare_socket error!\n");
			exit(1);
		}	

		if ( set_options() == 1){
			fprintf(stderr, "set_options error!\n");
			exit(1);
		}
	}

	private:
		int prepare_socket(){
			if ( (sock_fd = socket(AF_INET, SOCK_SEQPACKET, IPPROTO_SCTP)) == -1){
				fprintf(stderr,"socket error : %s\n", strerror(errno));
				return 1;
			}

			// Prepare local address structure
			bzero(&local_addr, sizeof(local_addr));
			local_addr.sin_family = af_family;
			local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
			local_addr.sin_port = htons(service);

			if( bind(sock_fd, (SA *) &local_addr, sizeof(local_addr)) == -1 ){
				fprintf(stderr,"bind error : %s\n", strerror(errno));
				return 1;
			}

			return 0;
		}

		int set_options(){
			bzero(&evnts, sizeof(evnts));
			evnts.sctp_data_io_event = 1;
			if(setsockopt(sock_fd, IPPROTO_SCTP, SCTP_EVENTS,
				   &evnts, sizeof(evnts)) == -1 ){
				fprintf(stderr,"setsockopt error : %s\n", strerror(errno));
				return 1;
			}

			return 0;
		}

		int listen_msg(){
			if ( listen(sock_fd, LISTENQ) == -1){
				fprintf(stderr,"listen error : %s\n", strerror(errno));
				return 1;
			}

			return 0;
		}

	protected:
		int recv_sctp(){
			size_t len = sizeof(struct sockaddr);

			if( (rd_sz = sctp_recvmsg(sock_fd, readbuf, sizeof(readbuf),
					 NULL, NULL,
					 &sri,NULL)) == -1) {
				fprintf(stderr,"sctp_recvmsg error : %s\n", strerror(errno));
				return -1;
			}

			return rd_sz;
		}

		int send_sctp(){
			if( (sctp_send(sock_fd, readbuf, rd_sz, 
					 &sri, 0)) < 0 ){
					fprintf(stderr,"sctp_sendmsg : %s\n", strerror(errno));
					return 1;
			}

			return 0;
		}

	public:
		int echo(){
			printf("Waiting for new clients..\n");

			if(listen_msg() != 0){
				fprintf(stderr, "listen_msg error\n");
			}

			while(true){
				if (recv_sctp() == -1){
					fprintf(stderr, "recv_send error\n");
					return 1;
				}

				printf("received: %s.\n", readbuf);

				if ( send_sctp() != 0 ){
					fprintf(stderr, "recv_send error: send_sctp\n");
					return 1;
				}

				printf("send: %s.\n", readbuf);
			}
		}
};

/**
 * Returns number of sctp streams in given sctp association.
 *
 * @param sock_fd Socket which the association belongs to.
 * @param assoc_id Association id of mentioned association.
 * @return sctp_status.sstat_outstrms Number of screamps per given assoc.
 */
int sctp_get_no_strms(int sock_fd, sctp_assoc_t assoc_id)
{
	socklen_t retsz;
	struct sctp_status status;
	retsz = sizeof(status);	
	bzero(&status,sizeof(status));

	status.sstat_assoc_id = assoc_id;
	if( sctp_opt_info(sock_fd,assoc_id, SCTP_STATUS,
		   &status, &retsz) <0 ){
		perror("sctp_get_no_strms:sctp_opt_info error");
		exit(-1);
	}

	return(status.sstat_outstrms);
};

int main(int argc, char **argv)
{
	BaseSCTP base_sctp(7777, AF_INET);
	base_sctp.echo();

	return 0;
}
