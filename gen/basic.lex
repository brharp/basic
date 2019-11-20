%{
#include "y.tab.h"
%}
%%

[0-9]*\.?[0-9]+(E-?[0-9]+)?	return NUMBER;

SIN	return SIN;
COS	return COS;
TAN	return TAN;
ATN	return ATN;
EXP	return EXP;
ABS	return ABS;
LOG	return LOG;
SQR	return SQR;
RND	return RND;
INT	return INT;
FN[A-Z]	return FN;

LET	return LET;
READ	return READ;
DATA	return DATA;
PRINT	return PRINT;
GOTO	return GOTO;
IF	return IF;
FOR	return FOR;
NEXT	return NEXT;
END	return END;
DEF	return DEF;
GOSUB	return GOSUB;
RETURN	return RETURN;
DIM	return DIM;
REM	return REM;
TO	return TO;
THEN	return THEN;
STEP	return STEP;
STOP	return STOP;

[A-Z][0-9]?	return ID;
".*?"	return LABEL;
<>	return NE;
>=	return GE;
<=	return LE;
\S	return yychar;
.	/* Ignore */

%%

