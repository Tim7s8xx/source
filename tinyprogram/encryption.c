#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <termio.h>
#define __ENCRYPTFILE__ "./tmpfile_encryption"
#define __DENCRYPTFILE__ "./tempfile_dencryption"
#define CH_NUM    1024
int getPasswd(char* passwd);
int encryption(char* path, char* key);
int dencryption(char* path, char* key, char type);
int main(int argc, char *argv[])
{
	char key[128];
	if (argc != 3) {
		printf("usage: encrypt <-d|-e|-s> <filename>\n");
		exit(-1);
	}
	printf("input key:\n");
	getPasswd(key);
	if (argv[1][1] == 'e')       
		encryption(argv[2], key);
	else if (argv[1][1] == 'd')
		dencryption(argv[2], key, 'd');
	else if (argv[1][1] == 's') /*only show text content*/
		dencryption(argv[2], key, 's');
	else
		printf("usage: encrypt <-d|-e> <filename>\n");
	return 0;   
}
int getPasswd(char* passwd)
{
	struct termio oldbuff, buff;
	ioctl(0, TCGETA, &oldbuff);
	buff = oldbuff;
	buff.c_lflag &= ~ECHO;
	if (ioctl(0, TCSETAF, &buff) != 0) {
		printf("error: fatal happen when closing echo\n");   
		ioctl(0, TCSETAF, &oldbuff);
		return -1;   
	}
	scanf("%s", passwd);
	ioctl(0, TCSETAF, &oldbuff);
	return 0;
}
void addhex(char* str1, long len, char* key)
{
	int keylen = strlen(key);
	int i,j;
	j = 0;
	for (i = 0; i < len; i++) {
		if (j == keylen)   
			j = 0;   
		str1[i]    += key[j++];
		str1[i] = (unsigned int)str1[i] % 128;
	}
}
void subhex(char* str1, long len, char* key)
{
	int keylen = strlen(key);
	int i,j;
	j = 0;
	for (i = 0; i < len; i++) {
		if (j == keylen)   
			j = 0;
		if (str1[i] < key[j])
			str1[i] = 128 - (unsigned int)key[j++] + (unsigned int)str1[i];
		else
			str1[i] -= key[j++];

	}
}
int encryption(char* path, char* key)
{
	char str[CH_NUM];
	char cmdline[CH_NUM];
	int rnum, wnum;
	FILE *fp_src, *fp_dst;

	fp_src = fopen (path, "r+b");
	if (fp_src == NULL) {
		printf("can't open %s\n", path);
		exit(-1);   
	}

	fp_dst = fopen (__ENCRYPTFILE__, "w+b");
	if (fp_dst == NULL) {
		printf("can't open %s\n", __ENCRYPTFILE__);   
		exit(-1);   
	}

	while ((rnum = fread(str, 1, CH_NUM-1, fp_src)) > 0) {
		addhex(str, rnum, key);   
		wnum = fwrite(str, 1, rnum, fp_dst);
		if (wnum != rnum) {
			printf("warning: lost data when writing\n");   
		}
	}           
	if (!feof(fp_src))
		printf("error: reading error\n");   

	fclose(fp_src);
	fclose(fp_dst);   
#ifdef SAFE_MODE
	sprintf(cmdline, "cp %s .%s_bak", path, path);
	system(cmdline);
#endif
	sprintf(cmdline, "mv %s %s", __ENCRYPTFILE__, path);
	system(cmdline);
}
int dencryption(char* path, char* key, char type)
{

	char str[CH_NUM];
	char cmdline[CH_NUM];
	int rnum, wnum;
	FILE *fp_src, *fp_dst;

	fp_src = fopen (path, "r+b");
	if (fp_src == NULL) {
		printf("can't open %s\n", path);
		exit(-1);   
	}
	if (type == 's')
		goto onlyshow;
	fp_dst = fopen (__DENCRYPTFILE__, "w+b");
	if (fp_dst == NULL) {
		printf("can't open %s\n", __DENCRYPTFILE__);   
		exit(-1);   
	}

	while ((rnum = fread(str, 1, CH_NUM-1, fp_src)) != 0) {
		subhex(str, rnum, key);   
		wnum = fwrite(str, 1, rnum, fp_dst);
		if (wnum != rnum) {
			printf("warning: lost data when writing\n");   
		}
	}           
	if (!feof(fp_src))
		printf("error: reading error\n");   

	fclose(fp_src);
	fclose(fp_dst);

	sprintf(cmdline, "mv %s %s", __DENCRYPTFILE__, path);
	system(cmdline);
	return 0;
onlyshow:
	while ((rnum = fread(str, 1, CH_NUM-1, fp_src)) > 0) {
		subhex(str, rnum, key);   
		str[rnum] = 0;
		printf("%s", str);   
	}           
	if (!feof(fp_src))
		printf("error: reading error\n");   
	fclose(fp_src);
	return 0;
}
