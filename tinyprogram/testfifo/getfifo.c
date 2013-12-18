#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#define MAX_LEN 1024
int main()
{
	int fd, fd1,n;
	char buf[MAX_LEN];
	fd = open("tmpfifo", O_RDONLY);
	if (fd < 0) {
		return 0;
	}
	while(1) {
		/********************
		  打开无数据的fifo，read会阻塞
		  但是，在setfifo退出后，fifo中
		  会填入eof文件结尾符，read不会
		  在读完数据后阻塞。
		  *******************/
		memset(buf, 0x0, MAX_LEN);
		n = read(fd, buf, MAX_LEN);
		if (n == 0) {
			sleep(3);
			fprintf(stderr,"n == 0,read eof");
		}
		else if ( n == -1) {
			perror("read:");
			exit(0);
		}
		buf[n] = 0;
		fprintf(stdout, "%s", buf);
		fflush(stdout);
		sleep(1);
	}
}
