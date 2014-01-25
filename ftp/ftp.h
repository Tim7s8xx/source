#ifndef FTP_H
#define FTP_H
#define MAX_IP  16
#define MAX_LINE 256
#define CMD_LINE 512

enum type_ {
	SEND_CMD = 0,
	RECV_CMD
};
enum cmd_type {
	REQUEST_SEND = 1,
	REQUEST_RECV,
	REQUEST_RM,
	UNKNOW
};
int writen(int fd, void* str, int len);

int readn(int fd, void* str, int len);

int ftp_send(int fd, int localfile, int sendsize);

int ftp_recv(int fd, int localfile, int recvsize);

int ftp_cmd(int fd, int type, int *cmd, unsigned long *size, char* path);

#endif
