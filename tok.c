#include <stdio.h>
#include <string.h>
#include <regex.h>

const char *tokens[] = {
	"10", "L", "ET", "X", "=", "X", "+", "1"
};

	//"[[:digit:]]*\\.?[[:digit:]]+(?:E-?[[:digit:]]+)?|" /* number */
const char *pattern =
	"[[:digit:]]+|" /* number */
	"SIN|COS|TAN|ATN|EXP|ABS|LOG|SQR|RND|INT|FN[A-Z]|" /* functions */
	"LET|READ|DATA|PRINT|GOTO|IF|FOR|NEXT|END|" /* keywords */
	"DEF|GOSUB|RETURN|DIM|REM|TO|THEN|STEP|STOP|" /* more keywords */
	"[A-Z][[:digit:]]?|" /* variable names (letter + optional digit) */
	"\".*?\"|" /* labels (strings in double quotes) */
	"<>|>=|<=|" /* multi-character relational operators */
	"[[:graph:]]" /* any non-space single character */
	;


int
parse()
{
	// skip spaces
	// if digit, read line number
	
	// read alpha
	// read second char
	// if second char is digit, its a variable
	//
}


int
split(char s[], char **p, int lim)
{
	int i, j, k = 0, n = 0;
        regex_t	re;
	regmatch_t rm = {0};

	if (regcomp(&re, pattern, REG_EXTENDED) != 0) {
		return(0);      /* Report error. */
	}
	for (i = 0; regexec(&re, &s[i], 1, &rm, 0) == 0; i += rm.rm_eo) {
		for (j = rm.rm_so; j < rm.rm_eo; j++, k++) {
			s[k] = s[i+j];
		}
	}
	s[k] = 0;
	for (i = 0; regexec(&re, &s[i], 1, &rm, 0) == 0 && n < lim-1; i += rm.rm_eo, n++) {
		p[n] = strndup(&s[i], rm.rm_eo - rm.rm_so);
	}
	p[n] = 0;
	regfree(&re);
	return(1);
}



/* Reads next character or keyword. */
int	getchr()
{
	static int pos = 0, cnt = 0;
	static char buf[80];
	int i, j, k;

	if (pos >= cnt) {
		if (fgets (buf, sizeof buf, stdin) == NULL)
			return EOF;
		pos = 0;
		for (i = 0, cnt = 0; regexec(&re, &buf[i], 1, &rm, 0) == 0; i += rm.rm_eo)
			for (j = rm.rm_so; j < rm.rm_eo; j++, cnt++)
				buf[cnt] = buf[i+j];
		buf[cnt] = 0;
	}

	if (regexec(&re, &buf[pos], 1, &rm, 0) == 0) {
		strncpy(s, &buf[pos+rm.rm_so], rm.rm_eo-rm.rm_so);
		pos += rm.rm_eo;
	}

	regfree(&re);

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

int	gettok(char *s, int lim)
{
	static int i = 0;
	if (i < sizeof(tokens)/sizeof(tokens[0])) {
		strcpy(s, tokens[i++]);
		return 1;
	}
	return 0;
}

int	tokenize()
{
	char	buf[80],
		tok[80];
	int	out = 0,
		i;

	while (gettok(tok, sizeof tok)) {
		printf("TOKEN: %s\n", tok);
		for (i = 0; i < sizeof tok - 1 && tok[i] != 0; i++, out++)
			buf[out] = tok[i];
	}

	buf[out] = 0;

	printf("Line: %s\n", buf);
}

int	main(int argc, char *argv[])
{
	char buf[80];
	char *tokens[80];
	int i;
	strcpy(buf, "10 L ET X=X+ 1\"GO TO\"");
	split(buf, tokens, 80);
	for (i = 0; tokens[i] != 0; i++) {
		printf("Token %d: %s\n", i, tokens[i]);
	}
}



