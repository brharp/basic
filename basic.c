/* BASIC */

#define _GNU_SOURCE 1

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>

#define TOKENMAX 80
#define INTSIZE 8

#define countof(X) (sizeof(X)/sizeof(X[0]))

/* Function declarations */

void getname (void);
void getnum (void);
void getop (void);
void getstr (void);
void init (void);
char *intern (char *);
int main (int argc, char *argv[]);
void next (void);
void nextchar (void);
void skipspace (void);
void syntaxerror (void);
/*int lookup (const char *table[], char *key, int n);*/
int kwsearch (void);
void scan (void);

/* Constants */

char *keywords[] = { "LET", "IF", "THEN", "GOTO", "FOR", "TO", 
  "STEP", "NEXT", "PRINT", "$TRING", "DIM", "INPUT" };
const char keycodes[] = "xlitgfosnp$dr"; 

/* Variables */

int  lookahead;
int  tokentype;
char tokentext[TOKENMAX+1];
int  tokenlength;
int  ln; /* Line number */

/* Code Templates */

#define ALOAD(ARRAY, INDEX) "\tmov\t%%rax, "ARRAY"[%%rip+"INDEX"]\n"
#define LOAD(X) "\tmov\t%%rax, "X"\n"
#define JUMP(LINENO) "\tjmp\tL"LINENO"\n"

/* Code generators */

#define CODEMAX 10000
int code[CODEMAX];
int *here = code;
enum op { STOP, ADD, FETCH, LABEL, LIT, PUSH, STORE, SUB };

void codegen(int op)
{
	*here++ = op;
}

void compile()
{
	int *op = code;
	while (*op)
	switch (*op++) {
	case ADD:
		printf("\tpop\t%%rdx\n");
		printf("\tadd\t%%rax, %%rdx\n");
		break;
	case FETCH:
		printf("\tmov\t%%rax, [VARBASE + %d * 8]\n", *op++);
		break;
	case LABEL:
		printf("L%d:\n", *op++);
		break;
	case LIT:
		printf("\tmov\t%%rax, %d\n", *op++);
		break;
	case PUSH:
		printf("\tpush\t%%rax\n");
		break;
	case STORE:
		//printf("\tpop\t%%rdx\n");
		printf("\tmov\t[VARBASE + %d * 8], %%rax\n", *op++); 
		break;
	case SUB:
		printf("\tmov\t%%rdx, %%rax\n");
		printf("\tpop\t%%rax\n");
		printf("\tsub\t%%rax, %%rdx\n");
		break;
	}
}

void run(int *op)
{
	int x, y;
	int VARBASE[26];
	int STACKBASE[16];
	int *sp = STACKBASE;
	while (1) {
		switch (*op++) {
		case ADD:
			*--sp = y;
			x += y;
			break;
		case FETCH:
			x = VARBASE[x];
			break;
		case LIT:
			x = *op++;
			break;
		case PUSH:
			*sp++ = x;
			break;
		case SUB:
			y = *--sp;
			x = y - x;
			break;
		}
		if (sp - STACKBASE > sizeof(STACKBASE)) {
			die("OVERFLOW");
		}
	}
}

void aload (const char *array, const int index)
{
  printf (ALOAD ("%1$s", "%2$d"), array, index);
}

void load (const char *x)
{
  printf (LOAD ("%1$s"), x);
}

void die (const char *msg)
{
  fprintf (stderr, "?%s IN %d\n", msg, ln);
  exit (EXIT_FAILURE);
}

void redimerror (void)
{
  die ("REDIM'D ARRAY");
}

void syntaxerror (void)
{
  die ("SYNTAX ERROR");
}

void nextchar (void)
{
  lookahead = getchar ();
}

void skipspace (void)
{
  while (isspace (lookahead))
    nextchar ();
}

void getname (void)
{
  tokentype = 'x';
  tokenlength = 0;
  while (isalnum (lookahead))
    {
      if (tokenlength > TOKENMAX)
        syntaxerror ();

      tokentext[tokenlength] = lookahead;
      tokenlength++;
      nextchar ();
    }
  tokentext[tokenlength] = '\0';
}
  
void getnum (void)
{
  tokentype = '#';
  tokenlength = 0;
  while (isdigit (lookahead))
    {
      if (tokenlength > TOKENMAX)
        syntaxerror ();
      tokentext[tokenlength] = lookahead;
      tokenlength++;
      nextchar ();
    }
  tokentext[tokenlength] = '\0';
}

void getstr (void)
{
  tokentype = '$';
  tokenlength = 0;
  assert (lookahead == '"');
  nextchar ();
  while (lookahead != '"')
    {
      if (tokenlength > TOKENMAX)
        syntaxerror ();
      tokentext[tokenlength] = lookahead;
      tokenlength++;
      nextchar ();
    }
  /* Eat closing quote. */
  nextchar ();
  tokentext[tokenlength] = '\0';
}

void getop (void)
{
  tokentype = lookahead;
  tokenlength = 1;
  tokentext[0] = lookahead;
  tokentext[1] = '\0';
  nextchar ();
}

void next (void)
{
  skipspace ();
  if (isalpha (lookahead)) getname ();
  else if (isdigit (lookahead)) getnum ();
  else if (lookahead == '"') getstr ();
  else getop ();
  scan ();
}

void init (void)
{
  nextchar ();
  next ();
}


int lookup (char *table[], char *key, int n)
{
  int i = n;
  while (i--)
    if (strcmp (table[i], key) == 0)
      break;
  return i;
}


int kwsearch (void)
{
  int nkw = countof (keywords);
  return keycodes[lookup (keywords, tokentext, nkw) + 1];
}
  
void scan (void)
{
  if (tokentype == 'x')
    tokentype = kwsearch ();
}

void match (int type)
{
  if (tokentype != type)
    syntaxerror ();
  next ();
}

/* Parse and translate a line number. */
void linenumber (void)
{
	/* Store current line number in global variable. */
	ln = atol(tokentext);
	/* Generate a line number label. */
	codegen(LABEL);
	codegen(ln);
	/* Match line number. */
	match ('#');
}

char *identifier (void);
int id (void);

void factor (void)
{
  if (tokentype == 'x')
    {
      //char *x = identifier ();
      int x = id ();
      //match ('x');
      if (tokentype == '(')
        {
          match ('(');
          int i = atoi (tokentext);
          match ('#');
          match (')');
          aload (x, i * INTSIZE);
        }
      else
        {
          //load (x);
          //codegen(LIT);
          //codegen(x);
          codegen (FETCH);
          codegen (x);
        }
    }
  else
    {
      codegen(LIT);
      codegen(atoi(tokentext));
      match ('#');
    }
}

void term (void)
{
  factor ();
  while (tokentype == '*' || tokentype == '/')
    {
      printf ("\tpush\t%%rax\n");
      switch (tokentype)
        {
          case '*':
            printf ("\tpop\t%%rdx\n");
            printf ("\timul\t%%rax, %%rdx\n");
            break;
          case '/':
            printf ("\tmov\t%%rcx, %%rax\n");
            printf ("\tpop\t%%rax\n");
            printf ("\tmov\t%%rdx, %%rax\n");
            printf ("\tsar\t%%rdx, 63\n");
            printf ("\tidiv\t%%rcx\n");
            break;
        }
    }
}

void expression (void)
{
  term ();
  while (tokentype == '+' || tokentype == '-')
    {
      codegen(PUSH);
      switch (tokentype)
        {
          case '+':
            match ('+');
            term ();
            codegen(ADD);
            break;
          case '-':
            match ('-');
            term ();
            codegen(SUB);
            break;
        }
    }
}


char *symboltable[128];
int symbolcount;

/* Allocate space for a variable. */
void allot (char *name, int size)
{
  printf ("\t.comm\t%s, %d\n", name, size);
}

/* Add name to symbol table. */
char *intern (char *name)
{
  //fprintf (stderr, "intern (\"%s\")\n", name);
  int i = symbolcount;
  /* Define symbol. */
  if (i >= countof(symboltable))
    die ("symbol table full");
  /* Add entry to table. */
  symboltable[i] = strdup (name);
  /* Increment symbol table counter. */
  ++symbolcount;
  //fprintf (stderr, "return \"%s\"\n", symboltable[i]);
  return symboltable[i];
}

void dim (void)
{
  match ('d');
  if (tokentype != 'x')
    syntaxerror ();
  int i = lookup (symboltable, tokentext, symbolcount);
  if (i > -1)
    redimerror ();
  char *x = intern (tokentext);
  match ('x');
  match ('(');
  if (tokentype != '#')
    syntaxerror ();
  int n = atol (tokentext);
  match ('#');
  match (')');
  allot (x, n*INTSIZE);
}

int id(void)
{
  /* Syntax check. */
  if (tokentype != 'x')
    syntaxerror ();
  /* Search for variable in symbol table. */
  int i = lookup (symboltable, tokentext, symbolcount);
  /* If not defined, insert into table. */
  if (i < 0)
    {
      i = symbolcount;
      /* Allocate space for variable. */
      //printf ("\t.comm\t%s, %d\n", tokentext, INTSIZE);
      /* Define symbol. */
      if (i >= countof(symboltable))
        die ("symbol table full");
      /* Add entry to table. */
      symboltable[i] = strdup (tokentext);
      /* Increment symbol table counter. */
      ++symbolcount;
    }
  //codegen(LIT);
  //codegen(i);
  /* Match variable. */
  match ('x');
  return i;
}

char *identifier(void)
{
	return symboltable[id()];
}

void assignment (void)
{
  /* Match (optional) "LET" keyword. */
  if (tokentype == 'l')
    match ('l');
  //char *id = identifier ();
  int i = id ();
  //codegen (PUSH);
  if (tokentype == '(')
    {
      match ('(');
      int offset = atoi (tokentext);
      match ('#');
      match (')');
      match ('=');
      expression ();
      printf ("\tmov\tQWORD PTR %s[%%rip+%d], %%rax\n", id, INTSIZE * offset);
    }
  else
    {
      match ('=');
      expression ();
      //printf ("\tmov\t%s, %%rax\n", id); 
      codegen (STORE);
      codegen (i);
    }
}

void statement (void);

/* Parse and translate a conditional (IF) statement. */
void conditional (void)
{
  /* Conditional jump label counter. */
  static int j = 100;
  /* Match the "IF" token. */
  match ('i'); 
  /* Match the first expression. */
  expression ();
  /* Push first value on the stack. */
  printf ("\tpush\t%%rax\n");
  /* Match relational operator. */
  if (tokentype == '<' || tokentype == '>' || tokentype == '=')
    {
      /* Conditional jump instruction. */
      char *jcc;
      /* Match 2-character operators such as <=, >=, <>, etc. */
      switch (tokentype)
        {
          /* Less then, less than or equal, not equal. */
          case '<':
            /* Match first character. */
            match ('<');
            /* Match (optional) second character. */
            switch (tokentype)
              {
                /* Not equal (<>). */
                case '>': match ('>'); jcc = "jz"; break;
                /* Less than or equal (<=). */
                case '=': match ('='); jcc = "jnle"; break;
                /* Less than (<). */
                default: jcc = "jnl"; break;
              }
            break;
          /* Greater than, greater than or equal, not equal. */
          case '>':
            /* Match first character. */
            match ('>');
            /* Match (optional) second character. */
            switch (tokentype)
              {
                /* Not equal (><). */
                case '<': match ('<'); jcc = "jz"; break;
                /* Greater than or equal (>=). */
                case '=': match ('='); jcc = "jnge"; break;
                /* Greater than (>). */
                default: jcc = "jng"; break;
              }
            break;
          /* Equal, less than or equal, greater or equal. */
          case '=':
            /* Match first character. */
            match ('=');
            /* Match (optional) second character. */
            switch (tokentype)
              {
                /* Less than or equal (=<). */
                case '<': match ('<'); jcc = "jnle"; break;
                /* Greater than or equal (=>). */
                case '>': match ('>'); jcc = "jnge"; break;
                /* Equal (=). */
                default: jcc = "jne"; break;
              }
            break;
        }
      /* Match second expression in comparison. */
      expression ();
      /* Pop first value into %rdx. */
      printf ("\tpop\t%%rdx\n");
      /* Compare expression values. */
      printf ("\tcmp\t%%rdx, %%rax\n");
      /* If the comparison fails, jump over the consequent. jcc is a
         conditional jump instruction set above (jne, jnge, etc.). */
      printf ("\t%s\tJ%d\n", jcc, j);
      /* Match "THEN" */
      match ('t');
      /* Compile the consequent; the statement to execute if the 
      condition is true. */
      statement ();
      /* Target label of conditional jump instruction. */
      printf ("J%d:\n", j++);
    }
}

/* Parse and translate a branch (aka GOTO). */
void branch (void)
{
  /* Match "GOTO" token. */
  match ('g');
  /* Convert line number token to int. */
  int lineno = atoi (tokentext);
  /* Match line number. */
  match ('#');
  /* Jump to line number label. */
  printf (JUMP ("%1$d"), lineno);
}

/* Parse and translate a (FOR) loop. */
void loop (void)
{
  void block (void);
  static int counter = 0;
  int label = counter++;
  /* Match "FOR" */
  match ('f');
  /* Match variable */
  char *id = identifier ();
  match ('=');
  /* Match lower bound */
  expression ();
  /* Initialize counter to lower bound */
  printf ("\tmov\t%s, %%rax\n", id);
  /* Match "TO" */
  match ('o');
  /* Match upper bound */
  expression ();
  printf ("\tpush\t%%rax\n");
  printf ("\tjmp\tT%d\n", label);
  printf ("N%d:\n", label);
  block ();
  /* Match "NEXT" */
  match ('n');
  if (tokentype != 'x' || strcmp (id, tokentext) != 0)
    syntaxerror ();
  match ('x');
  printf ("\tmov\t%%rax, %s\n", id);
  printf ("\tadd\t%%rax, 1\n");
  printf ("\tmov\t%s, %%rax\n", id);
  printf ("T%d:\n", label);
  printf ("\tmov\t%%rax, %s\n", id);
  printf ("\tcmp\t%%rax, [%%rsp]\n");
  printf ("\tjl\tN%d\n", label);
  printf ("\tadd\t%%rsp, %d\n", INTSIZE);
}

/* Match a variable reference. */
void var (void)
{
  char *x = identifier ();
  if (tokentype == '(')
    {
      match ('(');
      int i = atoi (tokentext);
      match ('#');
      match (')');
      printf ("\tlea\t%%rax, %s[%%rip+%d]\n", x, i * INTSIZE);
    }
  else
    {
      printf ("\tlea\t%%rax, %s\n", x);
    }
}

/* Input variable value. */
void input (void)
{
  /* Match INPUT token */
  match ('r'); 
  printf ("\tmov\t%%rax, 0\n"); /* read(2) */
  printf ("\tmov\t%%rdi, 0\n"); /* file descriptor = 0 */
  printf ("\tsub\t%%rsp, 2\n"); /* reserve space on stack */
  printf ("\tmov\t%%rsi, %%rsp\n"); /*  pointer to char (top of stack) */
  printf ("\tmov\t%%rdx, 1\n"); /* read 1 char */
  printf ("\tsyscall\n");
  printf ("\txor\t%%rbx, %%rbx\n"); /* clear rbx */
  printf ("\tpop\t%%dx\n"); /* Pop character read */
  printf ("\tmov\t%%bl, %%dl\n");
  var ();
  printf ("\tmov\t[%%rax], %%rbx\n"); /* store in variable */
}

/* Print variable value. */
void print (void)
{
  static int counter = 0;
  match ('p');
  /* Print a literal string. */
  if (tokentype == '$')
    {
      int label = counter++;
      printf ("\t.section .rodata\n");
      printf ("STR%d:\n", label);
      printf ("\t.string\t\"%s\"\n", tokentext);
      printf ("\t.section .text\n");
      printf ("\tmov\t%%rax, 1\n"); /* write(2) */
      printf ("\tmov\t%%rdi, 1\n"); /* file descriptor 1 */
      printf ("\tmov\t%%rsi, OFFSET FLAT: STR%d\n", label); /* ptr to char */
      printf ("\tmov\t%%rdx, %d\n", tokenlength); /* length of string */
      printf ("\tsyscall\n");
      printf ("\tcall\tnewline\n");
      match ('$');
    }
  /* Print an expression. */
  else
    {
      expression ();
      printf ("\tcall\tprint\n");
    }
}

/* Parse and translate a statement. */
void statement (void)
{
  /* Statements may begin with a line number. */
  if (tokentype == '#')
    /* Match line number. */
    linenumber ();
  /* Switch on first token. */
  switch (tokentype)
    {
      /* GOTO  */ case 'g': branch (); break;
      /* IF    */ case 'i': conditional (); break;
      /* FOR   */ case 'f': loop (); break;
      /* NEXT  */ case 'n': break;
      /* PRINT */ case 'p': print (); break;
      /* INPUT */ case 'r': input (); break;
      /* DIM   */ case 'd': dim (); break;
      /* LET   */ default: assignment (); break;
    }
}

/* Parse and translate a block of statements. */
void block (void)
{
  while (lookahead != EOF)
    {
      statement ();
      if (tokentype == 'n')
        break;
    }
}

/* Standard prologue. */
void prologue (void)
{
  printf ("\t.intel_syntax\n");
  printf ("\t.globl main\n");
  printf ("main:\n");
  printf ("\tpush\t%%rbp\n");
  printf ("\tmov\t%%rbp, %%rsp\n");
  printf ("\tsub\t%%rsp, 16\n");
  printf ("\tmov\tDWORD PTR [%%rbp-4], %%edi\n");
  printf ("\tmov\tQWORD PTR [%%rbp-16], %%rsi\n");
}

/* Standard epilogue. */
void epilogue (void)
{
  printf ("\tmov\t%%eax, 0\n");
  printf ("\tleave\n");
  printf ("\tret\n");
}

char *
match_keyword (char *first, char *keyword)
{
  if (*keyword == '\0')
    return first;
  else if (*first == *keyword)
    return match_keyword (first + 1, keyword + 1);
  else
    return NULL;
}

void
tokenize (void)
{
  char line[80];
  char *input = fgets (line, 80, stdin);
  int count = countof (keywords);
  while (*input) {
    int i;
    for (i = 0; i < count; i++) {
      char *p;
      if ((p = match_keyword (input, keywords[i]))) {
        printf ("TOKEN: %s\n", keywords[i]);
        input = p;
      }
    }
    if (i == count) {
      printf ("CHAR: %c\n", *input++);
    }
  }
}

/* Main program. */
int main (int argc, char *argv[])
{
  //for (;;)
    //tokenize();
  init ();
  prologue ();
  block ();
  epilogue ();
  compile();
  return 0;
}

