
%token NUMBER SIN COS TAN ATN EXP ABS LOG SQR RND INT FN LET READ
%token DATA PRINT GOTO IF FOR NEXT END DEF GOSUB RETURN DIM REM
%token TO THEN STEP STOP ID LABEL NE GE LE

%%

line: linenum stmt
    ;

stmt: let_stmt
    | read_stmt
    | data_stmt
    | print_stmt
    | goto_stmt
    ;

let_stmt: LET id = expr
	| ID = expr
	;

read_stmt: READ id
	 ;

data_stmt: DATA num
	 ;

print_stmt: PRINT expr
	  ;

goto_stmt: GOTO linenum
	 ;


