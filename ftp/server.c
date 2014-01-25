#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include "ftp.h"
int main(int argc, char* argv[])
{
	struct sockaddr_in in;
	struct in_addr s;
	int fd1, fd, localfile, len;
	int cmd, flag;
	unsigned long filesize = 0;
	struct stat st_buf;
	char path[MAX_LINE];
	in.sin_family = AF_INET;
	if (argc != 5) {
		printf("usage:server ip port path cmd\n");
		printf("cmd:1(recv),2(send)\n");
		return 0;
	}
	inet_pton(AF_INET, argv[1], &s);
	in.sin_addr = s;
	in.sin_port = htons(atoi(argv[2]));
	fd1 = socket(AF_INET, SOCK_STREAM, 0);
	if (fd1 < 0) {
		perror("socket");
		exit(-1);
	}

	if (setsockopt(fd1, SOL_SOCKET, SO_REUSEADDR, &len, sizeof(int)) < 0) {
		perror("setsockopt");
		exit(-1);
	}   
	
	if (bind(fd1, (struct sockaddr*)&in, sizeof(struct sockaddr)) < 0) {
		perror("bind");
		exit(-1);
	}   

	if (listen(fd1, 3) < 0) {
		perror("listen");
		exit(-1);
	}

	printf("server start...\n");
    if ((fd = accept(fd1, NULL, NULL)) < 0) {
		perror("accept");
		exit(-1);
	}
	cmd = atoi(argv[4]);
	if (cmd == REQUEST_RECV) {
		stat(argv[3], &st_buf);
		filesize = st_buf.st_size;
	}
	strcpy(path, argv[3]);
	cmd = ftp_cmd(fd, SEND_CMD, &cmd, &filesize, path);	
	cmd = ftp_cmd(fd, RECV_CMD, &cmd, &filesize, path);
	if (cmd == REQUEST_SEND) 
		flag = O_RDWR|O_CREAT|O_TRUNC;
	else if (cmd == REQUEST_RECV) 
		flag = O_RDONLY;
	else if (cmd == REQUEST_RM) {
		printf("remove[%s]:",path);
		fprintf(stderr, "file remove %s\n", path);
		unlink(path);
		goto end;
	}else {
		flag = O_RDWR;
	}

    localfile = open(path, flag, 0666);
    if (localfile == -1) {
        perror("open");
        exit(-1);
    }   
    if (cmd == REQUEST_SEND) {
		printf("recv[%s]:",path);
        if (ftp_recv(fd, localfile, filesize) == -1) 
            perror("ftp_recv");
    }else if (cmd == REQUEST_RECV) {
		printf("send[%s]:",path);
	    if (ftp_send(fd, localfile, filesize) == -1) 
            perror("ftp_send");
	} else{
		fprintf(stderr, "unknow command\n");
	}   
	char eof[6];
end:
	/* wait for client connect closed*/	
	read(fd, eof, 6);
	close(fd1);
	close(fd);
	close(localfile);
}
