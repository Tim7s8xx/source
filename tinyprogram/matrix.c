#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <time.h>
#include <pthread.h>
#include <signal.h>
#define TRUE 1
#define FALSE 0
pthread_t ntid[1024];
pthread_mutex_t mcursor[102];
pthread_mutex_t print_mutex = PTHREAD_MUTEX_INITIALIZER;
int tidnum = 0;
int mutexnum = -1;
int loopflag = TRUE;
struct pthread_arg {
	int ypos;
	int xsize;
	int mutex;
};
void clearcursor()
{
	printf("[2J");
}

void upcursor(int n)
{
	printf("[%dA", n);
}

void downcursor(int n)
{
	printf("[%dB", n);
}

void rightcursor(int n)
{
	printf("[%dC", n);
}

void leftcursor(int n)
{
	printf("[%dD", n);
}

void setposition(int x, int y)
{
	printf("[%d;%dH", x, y);
}

void sotrecursor()
{
	printf("[s");
}

void restorecursor()
{
	printf("[u");
}

void hidencursor()
{
	printf("[?25l");
}

void showcursor()
{
	printf("[?25h");
}
char randchar(int n)
{
	int a;
//	srand(n+time(NULL));
	a = rand();
	a = a % 127;
	if (a < 33)
		a = a + 33;
	return (char)a;
}
void* printchar(void *arg)
{
	struct pthread_arg* p = (struct pthread_arg*)arg;
	int i, row = p->xsize, ypos = p->ypos, mutex = p->mutex;
	while(loopflag) {
		/*give the mutex to the other thread*/
		sleep(1);
		pthread_mutex_lock(&mcursor[mutex]);
		for (i = 1;i <= row;i ++) {
			pthread_mutex_lock(&print_mutex);
			setposition(i, ypos);
			printf("%c", randchar(i*ypos));
			pthread_mutex_unlock(&print_mutex);
			usleep(50000);
		}
		pthread_mutex_unlock(&mcursor[mutex]);
	}
}
void stopthread(int signo)
{
	int i;
	signal(SIGINT, SIG_IGN);
	loopflag = FALSE;
	for (i = 0;i < 10;i ++) {
		clearcursor();
		printf("wait %d seconds......\r", 10 - i);
		fflush(stdout);
		sleep(1);
	}
}
int main()
{
	struct winsize size;
	struct pthread_arg parg;
	int j,k = 0;
	if (isatty(STDOUT_FILENO) == 0)
		exit(EXIT_FAILURE);

	if (ioctl(1, TIOCGWINSZ, (char*)&size) < 0 ) {
		perror("get winsize:");
		exit(EXIT_FAILURE);
	}
	setbuf(stdout, NULL);
	srand(time(NULL));
	clearcursor();
	for (j = 1;j <= size.ws_col;j = j + 3) {
			if (k%2 == 0) {
				mutexnum++;
				if (pthread_mutex_init(&mcursor[mutexnum], NULL) != 0) {
					printf("pthread_mutex_init error");
					mutexnum--;
					goto _EXIT_;
				}
			}
			parg.ypos = j;
			parg.xsize = size.ws_row;
			parg.mutex = mutexnum;
			if (pthread_create(&ntid[tidnum], NULL, printchar, &parg) != 0) {
				printf("pthread_create error");
				goto _EXIT_;
			}
			/*sleep or handle parg*/
			sleep(1);
			tidnum++;
			k++;
	}
	signal(SIGINT, stopthread);
	pause();
_EXIT_:
	for (k = 0; k <= mutexnum; k++) {
			pthread_mutex_destroy(&mcursor[k]);
	}
	exit(0);
}
