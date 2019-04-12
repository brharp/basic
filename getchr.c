
#include <stdio.h>
#include <ctype.h>
#include <assert.h>
#include <string.h>

#include "token.h"

/* Keywords. */

const char 	*kw[] = 
{
	"END", "FOR", "NEXT", "DATA", "INPUT", "DIM", "READ", "LET", "GOTO",
	"RUN", "IF", "RESTORE", "GOSUB", "RETURN", "REM", "STOP", "PRINT", "LIST",
	"CLEAR", "NEW", "TAB(", "TO", "THEN", "STEP",
};


/* Total number of keywords. */

const int	nkw = sizeof( kw ) / sizeof( kw[0] );


int	getchr()
{
	static int pos = 0, cnt = 0;
	static char buf[80];
	int i, j, k;

	if (pos >= cnt) {
		if (fgets (buf, sizeof buf, stdin) == NULL)
			return EOF;
		cnt = strlen (buf);
		pos = 0;
	}

	for (k = 0; k < nkw; k++) {
		for (i = pos, j = 0; kw[k][j] != 0; i++, j++) {
			while (isspace (buf[i]))
				++i;
			if (buf[i] != kw[k][j])
				break;
		}
		if (kw[k][j] == 0) {
			pos = i;
			return END + k;
		}
	}

	return buf[pos++];
}


#ifdef TEST
int main (int argc, char *argv[]) {
	int c;
	while ((c = getchr()) != EOF) {
		if (c < END)
			printf ("%c\t%x\n", c, c);
		else
			printf ("%s\t%x\n", kw[c-END], c);
	}
}
#endif

