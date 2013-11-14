#include <stdio.h>

#include "gera.h"

int main(void) {
	FILE *f;
	void * code;
	funcp fn;

	f = fopen("code.ltd","r");
	gera(f, &code, &fn);
	printf( "\nresultado: %d\n", (*fn)());
	libera(code);
	fclose(f);

	return 0;
}