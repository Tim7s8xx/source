#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
/* **************************************

   first cli up, next ser up 
   cli <remoteip> <port> <localip> <port>
   ser <remoteip> <port> <localip> <port>

 ****************************************/
int main(int argc, char* argv[])
{
	struct sockaddr cli,cli1,ser;
	int sockfd, i;
	time_t now;
	short port;
	socklen_t len;
	char str[1024], ipaddr[30];
	if (argc != 5)
		return 0;
	memset(&cli, 0x0 , sizeof(struct sockaddr));
	memset(&ser, 0x0, sizeof(struct sockaddr));
	init_cli_ser_struct(&cli, &ser, argv[1], (short)atoi(argv[2]), argv[3], (short)atoi(argv[4]));
	port = 0;
	memset(ipaddr, 0x0, 30);
	get_struct(&ser, ipaddr, 30, &port);
	printf("%s %d\n", ipaddr, port);
	sockfd = ntpestablish(&ser);
	if (sockfd == -1) {
		perror("ntpestablish err");
		return 0;
	}
	do {
		len = sizeof(struct sockaddr);
		if (ntprecv(str, sockfd, &cli1, &len) == -1) {
			perror("ntprecv err");
			return 0;
		}
		sleep(2);
		now = time(NULL);
		/* recv timestamp */
		sprintf(str+strlen(str),",%ld", now);
		/* send timestamp */
		now = time(NULL);
		sprintf(str+strlen(str),",%ld", now);
		if (ntpsend(str, sockfd, &cli, sizeof(cli)) == -1) {
			perror("ntpsend err");
			return 0;
		}
	}while(0);
	return 0;
}
