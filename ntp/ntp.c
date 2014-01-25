#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
int init_cli_ser_struct(struct sockaddr* cli, struct sockaddr* ser, 
								char* cliip, short cliport,
								char* serip, short serport)
{
	struct in_addr c, s;
	struct sockaddr_in* in;
	inet_pton(AF_INET, cliip, &c);
	inet_pton(AF_INET, serip, &s);
	in = (struct sockaddr_in *) cli;
	in->sin_family = AF_INET;
	in->sin_addr = c;
	in->sin_port = htons(cliport);
	
	in = (struct sockaddr_in *) ser;
	in->sin_family = AF_INET;
	in->sin_addr = s;
	in->sin_port = htons(serport);
	return 0;
}

int get_struct(struct sockaddr* addr, char* ipaddr, int len, short* port)
{
	struct sockaddr_in* in;
	in = (struct sockaddr_in *)addr;
	inet_ntop(AF_INET, &in->sin_addr, ipaddr, len);
	*port = ntohs(in->sin_port);
	return 0; 
}
int ntpestablish(struct sockaddr* seraddr)
{
	int sockfd, berr;
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd == -1) {
		return -1;
	}
	berr = bind(sockfd, seraddr, sizeof(struct sockaddr));
	if (berr == -1) {
		return -1;
	}
	return sockfd;
}

int ntpsend(char * str, int fd, struct sockaddr* cliaddr, socklen_t len)
{
	int err;
	err = sendto(fd, str, strlen(str)+1, 0, cliaddr, len);
	return err;
}

int ntprecv(char* str, int fd, struct sockaddr* cliaddr, socklen_t *len)
{
	int err;
	err = recvfrom(fd, str, 1024, 0, cliaddr, len);
	return err;
}

