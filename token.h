#ifndef _TOKEN_H_
#define _TOKEN_H_

enum 	keyword
{
	END = 128, FOR, NEXT, DATA, INPUT, DIM, READ, LET, GOTO,
	RUN, IF, RESTORE, GOSUB, RETURN, REM, STOP, PRINT, LIST,
	CLEAR, NEW, TAB, TO, THEN, STEP, IDENT, NUMBER, STRING,
	LINE, EOL, LT, LTE, GT, GTE, EQ, NEQ
};

int getchr();

#endif

