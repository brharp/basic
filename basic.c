/* BASIC */

#define _GNU_SOURCE 1

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#define TOKENMAX 8

#define countof(X) (sizeof(X)/sizeof(X[0]))

/* Function declarations */

void getname (void);
void getnum (void);
void getop (void);
void init (void);
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
  "STEP", "NEXT", "PRINT" };
const char keycodes[] = "xlitgfosnp"; 

/* Variables */

int  lookahead;
int  tokentype;
char tokentext[TOKENMAX+1];
int  tokenlength;

void die (const char *msg)
{
  fprintf (stderr, "%s\n", msg);
  exit (EXIT_FAILURE);
}

void syntaxerror (void)
{
  die ("syntax error");
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
        {
          syntaxerror ();
        }
      tokentext[tokenlength] = lookahead;
      tokenlength++;
      nextchar ();
    }
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
  /* Generate a line number label. */
  printf ("L%s:\n", tokentext);
  /* Match line number. */
  match ('#');
}

void factor (void)
{
  if (tokentype == 'x')
    {
      printf ("\tmov\t%%rax, %s\n", tokentext);
      match ('x');
    }
  else
    {
      printf ("\tmov\t%%rax, %s\n", tokentext);
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
      printf ("\tpush\t%%rax\n");
      switch (tokentype)
        {
          case '+':
            match ('+');
            term ();
            printf ("\tpop\t%%rdx\n");
            printf ("\tadd\t%%rax, %%rdx\n");
            break;
          case '-':
            match ('-');
            term ();
            printf ("\tmov\t%%rdx, %%rax\n");
            printf ("\tpop\t%%rax\n");
            printf ("\tsub\t%%rax, %%rdx\n");
            break;
        }
    }
}


char *symboltable[128];
int symbolcount;

char *identifier (void)
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
      printf ("\t.comm\t%s, 8\n", tokentext);
      /* Define symbol. */
      if (i >= countof(symboltable))
        die ("symbol table full");
      /* Add entry to table. */
      symboltable[i] = strdup (tokentext);
      /* Increment symbol table counter. */
      ++symbolcount;
    }
  /* Match variable. */
  match ('x');
  return symboltable[i];
}

void assignment (void)
{
  /* Match (optional) "LET" keyword. */
  if (tokentype == 'l')
    match ('l');
  char *id = identifier ();
  match ('=');
  expression ();
  printf ("\tmov\t%s, %%rax\n", id); 
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
  printf ("\tjmp\tL%d\n", lineno);
}

/* Parse and translate a (FOR) loop. */
void loop (void)
{
  void block (void);
  static int counter = 0;
  int label = counter++;
  match ('f');
  char *id = identifier ();
  match ('=');
  expression ();
  printf ("\tmov\t%s, %%rax\n", id);
  match ('o');
  expression ();
  printf ("\tpush\t%%rax\n");
  printf ("\tjmp\tT%d\n", label);
  printf ("N%d:\n", label);
  block ();
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
  printf ("\tadd\t%%rsp, 8\n");
}

/* Print variable value. */
void print (void)
{
  match ('p');
  if (tokentype != 'x')
    syntaxerror ();
  printf ("\tmov\t%%rax, %s\n", tokentext);
  printf ("\tcall\tprint\n");
  match ('x');
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
      /* GOTO */  case 'g': branch (); break;
      /* IF   */  case 'i': conditional (); break;
      /* FOR  */  case 'f': loop (); break;
      /* NEXT */  case 'n': break;
      /* PRINT */ case 'p': print (); break;
      /* LET  */  default: assignment (); break;
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

/* Main program. */
int main (int argc, char *argv[])
{
  init ();
  prologue ();
  block ();
  epilogue ();
  return 0;
}

