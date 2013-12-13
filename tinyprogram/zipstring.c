#include <stdio.h>
#include <string.h>

//void stringZip(const char *pInputStr, long lInputLen, char *pOutputStr);

void stringZip(const char *pInputStr, long lInputLen, char *pOutputStr)
{
	int repeat = 0;
	char cha;
	cha = *pInputStr;
	repeat = 1;
	while(*pInputStr++) {
		if (*pInputStr == cha)
			repeat++;
		else {
			if (repeat > 1){
/***************THIS IS VERY IMPORTANT PART*********************/
				sprintf(pOutputStr, "%d", repeat);
				while(*(++pOutputStr)!= 0);
				*pOutputStr = cha;/*remember over write the \0!*/
/***************THIS IS VERY IMPORTANT PART*********************/
			}
			*pOutputStr++ = cha;
			repeat = 1;
		}
		cha = *pInputStr;
	}
	*pOutputStr = 0;		
}

int main(int argc, char* argv[])
{
	if (argc != 2)
		return 0;
	char out[strlen(argv[1])+1];
	stringZip(argv[1], strlen(argv[1]), out);
	printf("out = %s\n", out);
}
