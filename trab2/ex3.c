#include <stdio.h>

typedef int (*funcp) ();

int main (void) {
	unsigned char codigo[] = { 0x55, 0x89, 0xe5, 0xb8, 0x0a, 0x00, 0x00, 0x00, 0x89, 0xec, 0x5d, 0xc3};
	funcp f = (funcp)codigo;
	printf("%d\n",(*f)());
	return 0;
}