#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>
#include "ftp.h"
int parse_cmd(char* str, int *cmd, unsigned long* filesize, char* path)
{
	char* strptr, *strptr_;
	strptr = strchr(str, ':');
	strptr_ = strchr(strptr+1, ':');
	/* replace ':' with '\0'*/
	*strptr = 0;strptr++;
	*strptr_ = 0;strptr_++;
	if (!strcmp(str, "REQUEST_SEND"))
		*cmd = REQUEST_SEND;
	else if (!strcmp(str, "REQUEST_RECV"))
		*cmd = REQUEST_RECV;
	else if (!strcmp(str, "REQUEST_RM"))
		*cmd = REQUEST_RM;
	else
		*cmd = UNKNOW;
	*filesize = atol(strptr);
	strcpy(path, strptr_);

	return *cmd;
}
int writen(int fd, void* str, int len)
{
	int cc, n = len;
	while(len) {
		cc = write(fd, str, len);
		if (cc <= 0) {
			if (cc < 0 && errno == EINTR)
				cc = 0;
			else
				return -1;
		}
		len -= cc;		
		str += cc;
	}
	return n;
}
int readn(int fd, void* str, int len)
{
	int cc, n = len;
	while(len) {
		cc = read(fd, str, len);
		if (cc == 0) {
			return (n - len);/* end of file */
		}else if (cc < 0) {
			if (errno == EINTR)
				continue;
			else
				return -1;
		}
		len -= cc;
		str += len;
	}
	return n;
}
/* fixed len cmd package */
/* format: REQUEST_XXX:file size:path */
int ftp_cmd(int fd, int type, int* cmd, unsigned long* len, char* path)
{
	char str[CMD_LINE], *ptr;
	int size;
	do {
		if (type) {
			memset(str, 0x0, CMD_LINE);
			size = readn(fd, str, CMD_LINE);
			if (size < CMD_LINE && size > 0) {
				fprintf(stderr, "reach the eof\n");
				return -1;
			}else if (size < 0) {
				perror("readn");	
				break;
			}
			if (str[0] != 'R' && str[0] != 'U')
				continue;
			str[size] = 0;
			parse_cmd(str, cmd, len, path);
		}
		else {
			if (*cmd == REQUEST_SEND)
				sprintf(str, "%s:%lu:%s", "REQUEST_SEND", *len, path);
			else if (*cmd == REQUEST_RECV)
				sprintf(str, "%s:%lu:%s", "REQUEST_RECV", *len, path);
			else if (*cmd == REQUEST_RM)
				sprintf(str, "%s:%lu:%s", "REQUEST_RM", *len, path);
			else 
				sprintf(str, "%s:%lu:%s", "UNKNOW", *len, path);
			size = writen(fd, str, CMD_LINE);
			if (size < 0) {
				perror("writen");
				break;
			}
		}
	}while(0);
	return *cmd;
}

int ftp_send(int fd, int localfile, int sendsize)
{
	int status = -1;
	ulong total = 0;
	char* buffer;
	int buffer_size;

	buffer = (char*)malloc(4*1024);
	buffer_size = 4*1024;
	memset(buffer, 0x0, buffer_size);

	while(1) {
		int rd;
		int wr;
		rd = read(localfile, buffer, buffer_size);
		if (!rd) {/* eof finish */
			status = 0;
			break;
		}
		if (rd < 0) {
			perror("read");
			break;
		}
		if (fd >= 0) {
			wr = writen(fd, buffer, rd);
			if (wr == -1) {
				perror("writen");
				break;
			}
		}
		if (wr != rd) {
			fprintf(stderr, "write from read error");
			break;
		}
		total += wr;
		printf("*");
		fflush(stdout);
	}
	printf("\n");
	free(buffer);
	return status?-1:total;
}

int ftp_recv(int fd, int localfile, int recvsize)
{
	int status = -1;
	ulong total = 0;
	char* buffer;
	int buffer_size, len;

	buffer = (char*)malloc(4*1024);
	buffer_size = 4*1024;
	memset(buffer, 0x0, buffer_size);

	while(1) {
		int rd;
		int wr;
		len = (buffer_size > recvsize?recvsize:buffer_size);
		rd = readn(fd, buffer, len);
		if (rd == -1) {
			perror("readn");
			break;
		}
		if (localfile >= 0) {
			wr = write(localfile, buffer, rd);
			if (wr < 0) {
				perror("writen");
				break;
			}
		}
		if (wr != rd) {
			fprintf(stderr, "write from read error");
			break;
		}
		total += wr;
		printf("*");
		fflush(stdout);
		recvsize -= rd;
		if (!recvsize) {/* read finish */
			status = 0;
			break;
		}

	}
	printf("\n");
	free(buffer);
	return status?-1:total;
}
