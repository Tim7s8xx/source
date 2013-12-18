#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
void cleanup(int flag)
{
	unlink("tmpfifo");
	fprintf(stderr, "cleanup tmpfifo\n");
	exit(0);
}
/********************
  重新打开的描述符1已经全缓冲，需fflush，
  否则需要等待标准i/o缓存满了，才能写入fifo
 ********************/
int main()
{
	time_t ttime;
	int fd, n;
	fd = mkfifo("tmpfifo", 0644);
	if (fd < 0)
		return 0;
	close(1);
	fd = open("tmpfifo", O_RDWR);
	if (fd < 0)
		return 0;
	fprintf(stderr, "fd = %d\n", fd);
	signal(SIGINT, cleanup);
	while (1) {
		ttime = time(NULL);
		fprintf(stdout, "time is %ld\n", (long) ttime);
		fflush(stdout);
		sleep(100);
	}
}
