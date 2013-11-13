#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef int (*funcp) ();

void parseRet( unsigned char *codeArray, int *codeCount, char *cmd, char* addrFill, int *addrFillCount )
{
	char ebpIncr, word[20];
	int * p, replace = 0;

	sscanf(cmd, " %*s %s", word);

	if(word[0] == '$' && atoi(&word[1]) != 0)
			return;

	if(word[0] != '$') {
		codeArray[(*codeCount)++]=0x83;
		codeArray[(*codeCount)++]=0x7d;
		if(word[0] == 'v')
			ebpIncr = (-4 * (atoi(&word[1])+1) );
		else
			ebpIncr = ( 4 * atoi(&word[1])) + 8;

		codeArray[(*codeCount)++] = (unsigned char)ebpIncr;
		codeArray[(*codeCount)++]=0x00;
		codeArray[(*codeCount)++]=0x75;
		replace = (*codeCount)++;
	}

	sscanf(cmd, " %*s %*s %s", word);
	if(word[0] == '$') {
		codeArray[(*codeCount)++]=0xb8;
		p=(int*)&codeArray[*codeCount];
		*p = atoi(&word[1]);
		*codeCount += 4;
	} else {
		codeArray[(*codeCount)++]=0x8b;
		codeArray[(*codeCount)++]=0x45;
		if(word[0] == 'v')
			ebpIncr = (-4 * (atoi(&word[1])+1) );
		else
			ebpIncr = ( 4 * atoi(&word[1])) + 8;

		codeArray[(*codeCount)++] = (unsigned char)ebpIncr;

	}
	
	codeArray[(*codeCount)++]=0xeb;
	codeArray[(*codeCount)++]=0x00;
	addrFill[ (*addrFillCount) ] = (*codeCount) - 1;
	(*addrFillCount)++;

	if(replace != 0) {
		codeArray[replace] = (unsigned char)((*codeCount) - replace - 1);
	}

}


void parseAssign( unsigned char *codeArray, int *codeCount, char * cmd ) {
	int *p;
	char assigned[10], operand[2][20], ebpIncr, operator;

	if(sscanf(cmd, " %s %*s %s %c %s", assigned, operand[0], &operator, operand[1]) != 4) return;

	// mov first operator to %ecx
	if(operand[0][0] == '$') {
		codeArray[(*codeCount)++]=0xb9;

		p=(int*)&codeArray[*codeCount];
		*p = atoi(&operand[0][1]);
		*codeCount += 4;
	} else {
		codeArray[(*codeCount)++]=0x8b;
		codeArray[(*codeCount)++]=0x4d;
		if(operand[0][0] == 'v')
			ebpIncr = (-4 * (atoi(&operand[0][1])+1) );
		else
			ebpIncr = ( 4 * atoi(&operand[0][1])) + 8;

		codeArray[(*codeCount)++] = (unsigned char)ebpIncr;
	}

	// add/sub/mul second operator to %ecx
	if(operand[1][0] == '$') {
		if(operator == '+') {
			codeArray[(*codeCount)++]=0x81;
			codeArray[(*codeCount)++]=0xc1;
		} else if (operator == '-') {
			codeArray[(*codeCount)++]=0x81;
			codeArray[(*codeCount)++]=0xe9;
		} else if (operator == '*') {
			codeArray[(*codeCount)++]=0x69;
			codeArray[(*codeCount)++]=0xc9;
		}
		p=(int*)&codeArray[*codeCount];
		*p = atoi(&operand[1][1]);
		*codeCount += 4;
	} else {
		if(operator == '+') {
			codeArray[(*codeCount)++]=0x03;
			codeArray[(*codeCount)++]=0x4d;
		} else if (operator == '-') {
			codeArray[(*codeCount)++]=0x2b;
			codeArray[(*codeCount)++]=0x4d;
		} else if (operator == '*') {
			codeArray[(*codeCount)++]=0x0f;
			codeArray[(*codeCount)++]=0xaf;
			codeArray[(*codeCount)++]=0x4d;
		}
		if(operand[1][0] == 'v')
			ebpIncr = (-4 * (atoi(&operand[1][1])+1) );
		else
			ebpIncr = ( 4 * atoi(&operand[1][1])) + 8;

		codeArray[(*codeCount)++] = (unsigned char)ebpIncr;
	}

	// mov %ecx result value to assigned variable
	codeArray[(*codeCount)++]=0x89;
	codeArray[(*codeCount)++]=0x4d;
	if(assigned[0] == 'v')
		ebpIncr = (-4 * (atoi(&assigned[1])+1) );
	else
		ebpIncr = ( 4 * atoi(&assigned[1])) + 8;

	codeArray[(*codeCount)++] = (unsigned char)ebpIncr;
}


void parseCall( unsigned char *codeArray, int *codeCount, char * cmd, unsigned char ** functions ) {
	char assigned[10], param[10], ebpIncr;
	int functionId, *p;

	if(sscanf(cmd, " %s %*c %*s %d %s", assigned, &functionId, param) != 3) return;
	if(param[0] == '$') {
		codeArray[(*codeCount)++]=0x68;
		p=(int*)&codeArray[*codeCount];
		*p = atoi(&param[1]);
		*codeCount += 4;
	} else {
		codeArray[(*codeCount)++]=0xff;
		codeArray[(*codeCount)++]=0x75;
		if(param[0] == 'v')
			ebpIncr = (-4 * (atoi(&param[1])+1) );
		else
			ebpIncr = ( 4 * atoi(&param[1])) + 8;

		codeArray[(*codeCount)++] = (unsigned char)ebpIncr;
	}

	codeArray[(*codeCount)++]=0xe8;
	p=(int*)&codeArray[*codeCount];
	*codeCount += 4;
	*p = functions[functionId] - &codeArray[*codeCount];

	codeArray[(*codeCount)++]=0x89;
	codeArray[(*codeCount)++]=0x45;	

	if(assigned[0] == 'v')
		ebpIncr = (-4 * (atoi(&assigned[1])+1) );
	else
		ebpIncr = ( 4 * atoi(&assigned[1])) + 8;

	codeArray[(*codeCount)++] = (unsigned char)ebpIncr;

}


void parseFunction(FILE *f, unsigned char * codeArray, int *codeCount, unsigned char ** functions)
{
	char cmd[100], word[20], addrFill[20];
	int i, addrFillCount = 0;

	// Function init
	codeArray[(*codeCount)++]=0x55;
	codeArray[(*codeCount)++]=0x89;
	codeArray[(*codeCount)++]=0xe5;
	codeArray[(*codeCount)++]=0x83;
	codeArray[(*codeCount)++]=0xc4;
	codeArray[(*codeCount)++]=0x28;
	//

	while( fscanf(f, " %[^\n]", cmd) == 1 && strcmp(cmd, "end") != 0)
	{
		
		// parse ret
		if( strncmp(cmd, "ret", 3) == 0 ) {
			parseRet( codeArray, codeCount, cmd, addrFill, &addrFillCount );
		}
		//

		// parse call
		else if( sscanf(cmd, " %*s %*s %s", word) == 1 && strncmp(word, "call", 4) == 0 ) {
			parseCall( codeArray, codeCount, cmd, functions);
		}
		//

		// parse simple assignment
		else if( sscanf(cmd, " %*s %s", word) == 1 && word[0] == '=' ) {
			parseAssign( codeArray, codeCount, cmd );
		}
		//

		//
		else {
			printf("--- LINHA IGNORADA ---\n> %s\n\n", cmd);
		}

	}
	
	for(i = 0; i < addrFillCount; i++) {
		codeArray[(int)addrFill[i]] = (unsigned char)( (*codeCount) - addrFill[i] - 1);
	}

	//for(i = 6; i < *codeCount; i++)
	//	printf("> %02x\n",codeArray[i]);

	// Function end
	codeArray[(*codeCount)++]=0x89;
	codeArray[(*codeCount)++]=0xec;
	codeArray[(*codeCount)++]=0x5d;
	codeArray[(*codeCount)++]=0xc3;
	//
}

void gera(FILE *f, void **code, funcp *entry)
{
	//unsigned char codeArray[200];
	unsigned char* functions[10], *codeArray;
	int codeCount = 0, fnCount = 0;
	char word[20];

	rewind( f );

	codeArray = (unsigned char *)malloc(500);

	while( fscanf(f, " %s", word) == 1 && strcmp(word, "function") == 0)
	{
		functions[fnCount++] = &codeArray[codeCount];
		parseFunction(f, codeArray, &codeCount, functions);
	}

	*code = (void *)codeArray;
	*entry = (funcp)functions[fnCount-1];
}

void libera(void *p)
{
	free(p);
}

int main(void) {
	FILE *f;
	void * code;
	funcp fn;

	f = fopen("code.ltd","r");
	gera(f, &code, &fn);
	printf( "\nresultado: %d\n", (*fn)());
	//libera(code);
	fclose(f);

	return 0;
}
