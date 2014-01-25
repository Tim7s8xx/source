#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "ftp.h"
int main(int argc, char* argv[])
{
	struct sockaddr_in in;
	struct in_addr s;
	int fd, localfile, cmd, flag;
	unsigned long filesize = 0;
	struct stat st_buf;
	char path[MAX_LINE];
	if (argc != 3) {
		printf("client ip port\n");
		return 0;
	}
	in.sin_family = AF_INET;
	inet_pton(AF_INET, argv[1], &s);
	in.sin_addr = s;
	in.sin_port = htons(atoi(argv[2]));
	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd < 0) {
		perror("socket");
		exit(-1);
	}

	if (connect(fd, (struct sockaddr*)&in, sizeof(struct sockaddr)) < 0) {
         perror("connect");
		 exit(-1);
	}
	printf("connect to server...\n");

	cmd = ftp_cmd(fd, RECV_CMD, &cmd, &filesize, path);
	if (cmd == REQUEST_SEND) {
		stat(path, &st_buf);
		filesize = st_buf.st_size;
	}
	cmd = ftp_cmd(fd, SEND_CMD, &cmd, &filesize, path);
	if (cmd == REQUEST_SEND) 
		flag = O_RDONLY;
	else if (cmd == REQUEST_RECV)
		flag = O_RDWR|O_CREAT|O_TRUNC;
	else if (cmd == REQUEST_RM) {
		printf("remove[%s]:",path);
		fprintf(stderr, "remove file %s\n", path);
		unlink(path);
		goto end;
	}else
		flag = O_RDWR;
	localfile = open(path, flag, 0666);
	if (localfile == -1) {
		perror("open");
		exit(-1);
	}
	if (cmd == REQUEST_SEND) {
		printf("send[%s]:",path);
		if (ftp_send(fd, localfile, filesize) == -1)
			perror("ftp_send");
	}else if (cmd == REQUEST_RECV) {
		printf("recv[%s]:",path);
		if (ftp_recv(fd, localfile, filesize) == -1)
			perror("ftp_recv");
	}else {
		fprintf(stderr, "unknow command\n");
	}
end:
	close(fd);
	close(localfile);
}
