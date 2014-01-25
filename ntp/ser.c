#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>

int main(int argc, char* argv[])
{
	struct sockaddr cli,cli1,ser;
	time_t now,T1,T2,T3,T4;
	int sockfd;
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
		/* original timestamp */
		now = time(NULL);
		sprintf(str, "%ld", now);
		if (ntpsend(str, sockfd, &cli, sizeof(cli)) == -1) {
			perror("ntpsend err");
			return 0;
		}
		len = sizeof(struct sockaddr);
		memset(str, 0x0, 1024);
		if (ntprecv(str, sockfd, &cli1, &len) == -1) {
			perror("ntprecv err");
			return 0;
		}
		/* recv timestamp */
		sleep(1);
		now = time(NULL);
		sprintf(str+strlen(str), ",%ld", now);
		sscanf(str,"%ld,%ld,%ld,%ld", &T1,&T2,&T3,&T4);
		printf("%ld,%ld,%ld,%ld\n", T1,T2,T3,T4);
		printf("2*delay:(%ld) diffcs(%ld)\n", (T2-T1)+(T4-T3), ((T2-T1)-(T4-T3))/2);
		get_struct(&cli1, ipaddr, 30, &port);
		printf("recive from %s:%d\n %s\n", ipaddr, port, str);
	}while(0);
	return 0;
}
