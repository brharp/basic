
#include <stdio.h>
#include <ctype.h>
#include <assert.h>
#include <string.h>

#include "token.h"

/* Character constants */
#define	QUOTE	'"'
#define NEWLINE '\n'

#define ID_MAX 16
#define STR_MAX 1000

/* Keywords. */

const char 	*kw[] = 
{
	"END", "FOR", "NEXT", "DATA", "INPUT", "DIM", "READ", "LET", "GOTO",
	"RUN", "IF", "RESTORE", "GOSUB", "RETURN", "REM", "STOP", "PRINT", "LIST",
	"CLEAR", "NEW", "TAB(", "TO", "THEN", "STEP",
};


/* Total number of keywords. */

const int	nkw = sizeof( kw ) / sizeof( kw[0] );

/* Input character buffer. */
static int	ch = '\0';

/* Symbol */
enum keyword	sym;

/* Line number */
int	nb;

/* Identifier */
char	id[16];

/* String literal */
char	str[4096];

/* Numeric value */
float	val;

/* Reads next character or keyword. */
int	getchr()
{
	static int pos = 0, cnt = 0, qmode = 0;
	static char buf[80];
	int i, j, k;

	if (pos >= cnt) {
		if (fgets (buf, sizeof buf, stdin) == NULL)
			return EOF;
		cnt = strlen (buf);
		pos = 0;
	}

	if (buf[pos] == QUOTE) {
		qmode = !qmode;
		return buf[pos++];
	}

	if (qmode) {
		return buf[pos++];
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


/* Reads an identifier. */
void	ident (void)
{
	int i = 0;

	sym = IDENT;

	while ( isalnum ( ch ) )
		{
			if ( i < sizeof id )
				{
					id [ i ++ ] = ch;
				}

			ch = getchr ();
		}

	id [ i ] = '\0';
}
 
/* Reads a number. */
void	number (void)
{
	sym = NUMBER;
	val = 0;
	while (isdigit (ch))
		{
			val = val * 10 + (ch - '0');
			ch = getchr();
		}
}

/* Reads a line number. */
void	linenum ( void )
{
	number () ;
	sym = LINE ;
	nb = (int) val ;
}

/* Reads a string. */
void	string (void)
{
	int len = 0;

	sym = STRING;

	assert ( ch == '"' ) ;
	ch = getchr () ;

	while ( ch != '"' )
		{
			if ( len < sizeof str )
				str [ len ++ ] = ch ;

			ch = getchr () ;
		}

	/* Null terminate string. */
	str [ len ] = '\0' ;

	/* Eat closing quote. */
	ch = getchr () ;
}

int	getsym()
{
	static int bol = 1;

	while ( ch == '\0' )
		ch = getchr () ;

		if ( ch == NEWLINE )
			{
				bol = 1;
				sym = EOL;
				ch = getchr () ;
			}
		else if ( bol && isdigit ( ch ) )
			{
				bol = 0 ;
				linenum () ;
			}
		else if ( isdigit ( ch ) )
			{
				number () ;
			}
		else if ( isalpha ( ch ) )
			{
				ident () ;
			}
		else if ( ch == QUOTE )
			{
				string () ;
			}
		else
			{
				sym = ch ;
				ch = getchr () ;
			}

	return sym ;
}

#ifdef TEST
int main (int argc, char *argv[]) {
	int sym;
	while ((sym = getsym()) != EOF) {
		switch (sym) {
		case STRING:
			printf("STRING\t\"%s\"\n", str);
			break;
		case NUMBER:
			printf("NUMBER\t%f\n", val);
			break;
		case IDENT:
			printf("IDENT\t%s\n", id);
			break;
		case LINE:
			printf("LINE\t%d\n", nb);
			break;
		case EOL:
			printf("EOL\n");
			break;
		default:
			if ( sym >= END )
				printf("KEYWORD\t%s\n", kw[sym - END]);
			else if ( isprint ( sym ) )
				printf("CHAR\t'%c'\n", sym);
			else
				printf("CHAR\t(%d)\n", sym);
			break;
		}
	}
}
#endif

