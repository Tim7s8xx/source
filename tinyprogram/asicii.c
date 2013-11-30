#include <stdio.h>
#include <string.h>
typedef struct ctlcharacter
{
	int value;
	char name[4];
	char desc[40];
} ctlcharacter;
int main(int argc, char* argv[])
{
	int a;
	/*invisible control character*/
	ctlcharacter ctlcha[] =
	{   
		{0, "NUL", "null"},
		{1, "SOH", "start of heading"},
		{2, "STX", "start of text"},
		{3, "ETX", "end of text"},
		{4, "EOT", "end of transmission"},
		{5, "ENQ", "enquiry"},
		{6, "ACK", "acknowledge"},
		{7, "BEL", "bell"},
		{8, "BS", "backsapce"},
		{9, "TAB", "horizontal tab"},
		{10, "LF", "NL line feed, new line"},
		{11, "VT", "vertical tab"},
		{12, "FF", "NP form feed, new page"},
		{13, "CR", "carriage return"},
		{14, "SO", "shift out"},
		{15, "SI", "shift in"},
		{16, "DLE", "data link escape"},
		{17, "DC1", "device control 1"},
		{18, "DC2", "device control 2"},
		{19, "DC3", "device control 3"},
		{20, "DC4", "device control 3"},
		{21, "NAK", "negative acknowledge"},
		{22, "SYN", "synchronous idle"},
		{23, "ETB", "end of trans. block"},
		{24, "CAN", "cancel"},
		{25, "EM", "end of medium"},
		{26, "SUB", "substitute"},
		{27, "ESC", "escape"},
		{28, "FS", "file separator"},
		{29, "GS", "group separator"},
		{30, "RS", "record separator"},
		{31, "US", "unit separator"},
		{32, "SP", "space"},
		{127, "DEL", "del"}
	};
	ctlcharacter *ctlptr = ctlcha;
	int onlyctl = 0;
	if (argc >= 2) {
		if (strcmp(argv[1], "-c") !=0) {
			fprintf(stderr, "only display control usage:%s -c\n", argv[0]);
			return 0;
		}
		onlyctl = 1;
	}
	else
		onlyctl = 0;

	for (a = 0; a < 128; a++) {
		if (onlyctl == 0) {
			if (a%4 == 0 && a != 0)
				printf("\n");
			if (ctlptr->value == a) {
				printf("%s[%d|0x%x]\t", ctlptr->name, a, a);
				ctlptr++;
			}
			else
				printf("%c[%d|0x%x]\t",a,a,a);
		}
		else {
			if (ctlptr->value == a) {
				printf("%s[%d|0x%x][%s]\n", ctlptr->name, a, a, ctlptr->desc);
				ctlptr++;
			}
		}
	}
	printf("\n");
	return 0;
}
