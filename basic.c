
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

void prologue ();
void epilogue ();

char *KEYWORDS = "IF\x81" "ELSE\x82" "LET\x83" "GOTO\x84";

#define IF   0x81
#define ELSE 0x81
#define LET  0x83
#define GOTO -124

int
is_addop (char c)
{
  return (c == '+' || c == '-');
}

int
is_mulop (char c)
{
  return (c == '*' || c == '/');
}

/*
 * ERROR UNIT
 */

void
syntax_error ()
{
  fprintf (stderr, "? Syntax Error\n");
  exit (EXIT_FAILURE);
}

void
out_of_memory ()
{
  fprintf (stderr, "Out of memory\n");
  exit (EXIT_FAILURE);
}

void
undefined (char *name)
{
  fprintf (stderr, "Undefined variable '%s'\n", name);
  exit (EXIT_FAILURE);
}


/* SYMBOL TABLE */

struct symbol
{
  char *name;
  struct symbol *next;
};

/* The symbol table. */
static struct symbol *symbol_table = NULL;
void allot (char *);

struct symbol *
lookup (char *name)
{
  struct symbol *sp;
  for (sp = symbol_table; sp != NULL; sp = sp->next)
    if (strcmp (sp->name, name) == 0)
      break;
  if (sp == NULL)
    {
      allot (name);
      sp = calloc (1, sizeof (*sp));
      if (sp == NULL)
	out_of_memory ();
      sp->name = strdup (name);
      if (sp->name == NULL)
	out_of_memory ();
      sp->next = symbol_table;
      symbol_table = sp;
    }
  return sp;
}



/*
 * THE TOKENIZER
 *
 * The tokenizer pre-parses the line buffer into tokens. It replaces
 * keywords from the input with single character tokens, and removes
 * spaces. Quoted strings are not tokenized.
 *
 * Tokenizing input simplifies parsing later, because we can assume
 * that any input starting with an alphabetic character is an
 * identifier (subroutine, function, or variable) without having to
 * check for keywords, and we can recognize keywords with a switch
 * statement or simple comparison.
 *
 */

/* ------------------------------------------------------------------ */
/* Tokenize a string. */

void
Tokenize (char *s)
{
  char *d = s;
  /* Scan the input string. Replace keywords with tokens.      */
  /* Skip over spaces. Do not scan for keywords inside quotes. */
  while (*s)
    {
      if (*s == ' ')
	{
	  s++;
	}
      else if (*s == '"')
	{
	  char c, t = *s;
	  *d = *s;
	  ++d;
	  ++s;
	  while (*s)
	    {
	      c = *s;
	      *d = c;
	      ++d;
	      ++s;
	      if (c == t)
		break;
	    }
	}
      else
	{
	  char *k = KEYWORDS;
	  int iskey = 0;
	  /* While more keywords to search: */
	  while (*k)
	    {
	      int i = 0;
	      /* While not at end of string or keyword: */
	      while (s[i] && k[i] > 0)
		{
		  /* If input does not match keyword: */
		  if (s[i] != k[i])
		    break;
		  i++;
		}
	      /* Keyword found? */
	      if (k[i] < 0)
		{
		  iskey = 1;
		  /* Write keyword token. */
		  *d++ = k[i];
		  /* Skip over keyword. */
		  s += i;
		  break;
		}
	      /* Next keyword. */
	      while (*k > 0)
		k++;
	      if (*k)
		k++;
	    }
	  /* If not keyword, copy character. */
	  if (!iskey)
	    {
	      *d++ = *s++;
	    }
	}			/* end if */
    }				/* end while */
  /* Null terminate tokenized string. */
  *d = '\0';
}

void
Dump (char *s)
{
  char *p;
  for (p = s; *p; p++)
    fprintf (stderr, "\t%c\t$%hhx\t%d\n", *p, *p, *p);
}

int Look;

/* Gets a non-blank line from stdin. */
void
get_line (char *s, int size)
{
  static int line_number = 0;
  do
    {
      s = fgets (s, size, stdin);
      if (s == NULL)
	exit (EXIT_SUCCESS);
      fprintf (stderr, "%d: %s\n", ++line_number, s);
    }
  while (!strcspn (s, " \t\n"));
}

int
GetChar ()
{
  static char LineBuffer[80];
  static int LinePos = 0;
  static int LineLength = 0;
  if (!(LinePos < LineLength))
    {
      get_line (LineBuffer, sizeof (LineBuffer) - 1);
      Tokenize (LineBuffer);
      Dump (LineBuffer);
      LineLength = strlen (LineBuffer);
      LinePos = 0;
    }
  Look = LineBuffer[LinePos++];
  return 1;
}

void
match (char c)
{
  if (Look != c)
    {
      fprintf (stderr, "Expected %c\n", c);
      exit (EXIT_FAILURE);
    }
  GetChar ();
}

void
Expected (char *what)
{
  printf ("Expected %s\n", what);
  exit (EXIT_FAILURE);
}

char *
get_name ()
{
  static char s[16];
  int i = 0;
  if (!isalpha (Look))
    syntax_error ();
  while (isalnum (Look))
    {
      s[i++] = Look;
      GetChar ();
    }
  s[i] = '\0';
  return s;
}

char *
get_var ()
{
  char name[8];
  int len = 0;
  if (!isalpha (Look))
    syntax_error ();
  while (isalnum (Look))
    {
      name[len++] = Look;
      GetChar ();
    }
  name[len] = '\0';
  struct symbol *sp = lookup (name);
  return sp->name;
}

int
get_number ()
{
  char s[16];
  char *p = s;
  if (!isdigit (Look))
    Expected ("Integer");
  while (isdigit (Look))
    {
      *p++ = Look;
      GetChar ();
    }
  return atoi (s);
}


/* CODE GENERATION */

/* Load constant N into primary register. */
void
load_constant (int n)
{
  printf ("\tmov\t%%rax, %d\n", n);
}

/* Load a variable into primary register. */
void
load_variable (char *name)
{
  printf ("\tmov\t%%rax, %s\n", name);
}

/* Store primary value into variable.  */
void
store (char *name)
{
  printf ("\tmov\t%s, %%rax\n", name);
}

/* Push primary register to stack. */
void
push ()
{
  printf ("\tpush\t%%rax\n");
}

/* Divide top of stack by primary value. */
void
pop_div ()
{
  printf ("\tmov\t%%rcx, %%rax\n");
  printf ("\tpop\t%%rax\n");
  printf ("\tmov\t%%rdx, %%rax\n");
  printf ("\tsar\t%%rdx, 63\n");
  printf ("\tidiv\t%%rcx\n");
}

/* Multiply primary value by top of stack. */
void
pop_mul ()
{
  printf ("\tpop\t%%rdx\n");
  printf ("\timul\t%%rax, %%rdx\n");
}

/* Subtract primary from top of stack.  */
void
pop_sub ()
{
  printf ("\tmov\t%%rdx, %%rax\n");
  printf ("\tpop\t%%rax\n");
  printf ("\tsub\t%%rax, %%rdx\n");
}

/* Add top of stack to primary.  */
void
pop_add ()
{
  printf ("\tpop\t%%rdx\n");
  printf ("\tadd\t%%rax, %%rdx\n");
}

/* Allocate space for a variable. */
void
allot (char *name)
{
  printf ("\t.comm\t%s, 8\n", name);
}

/* Unconditional jump to label. */
void
jump (int line_number)
{
  printf ("\tjmp\tL%d\n", line_number);
}


/* PARSER */

void add ();
void subtract ();
void multiply ();
void divide ();
void factor ();
void term ();
void expression ();

void
add ()
{
  push ();
  match ('+');
  term ();
  pop_add ();
}

void
subtract ()
{
  push ();
  match ('-');
  term ();
  pop_sub ();
}

/* Parse and translate multiplication of two terms.  */
void
multiply ()
{
  push ();
  match ('*');
  factor ();
  pop_mul ();
}

/* Parse and translate division of two terms.  */
void
divide ()
{
  push ();
  match ('/');
  factor ();
  pop_div ();
}

/* Parse and translate a factor. */
void
factor ()
{
  if (isalpha (Look))
    load_variable (get_var ());
  else if (isdigit (Look))
    load_constant (get_number ());
  else
    syntax_error ();
}

/* Parse and translate a term.  */
void
term ()
{
  factor ();
  while (is_mulop (Look))
    {
      switch (Look)
	{
	case '*':
	  multiply ();
	  break;
	case '/':
	  divide ();
	  break;
	}
    }
}

/* Parse and translate an expression.  */
void
expression ()
{
  term ();
  while (is_addop (Look))
    {
      switch (Look)
	{
	case '+':
	  add ();
	  break;
	case '-':
	  subtract ();
	  break;
	}
    }
}

/* Parse and translate an assignment statement.  */
void
assignment ()
{
  char *var = get_var ();
  match ('=');
  expression ();
  store (var);
}

/* Parse and translate a label. */
void
label ()
{
  int n = get_number ();
  printf ("L%d:\n", n);
}

/* Parse and translate a goto statement. */
void
goto_stmt ()
{
  int lineno;
  match (GOTO);
  lineno = get_number ();
  jump (lineno);
}

/* Parse and translate an if statement. */
void
if_stmt ()
{
}

/* Parse and translate a statement. */
void
statement ()
{
  if (isdigit (Look))
    label ();
  switch (Look)
    {
    case 0x81:
      if_stmt ();
      break;
    case GOTO:
      goto_stmt ();
      break;
    default:
      assignment ();
      break;
    }
  match ('\n');
}

/* Parse and translate a program.  */
void
program ()
{
  while (Look != EOF)
    {
      statement ();
    }
}

/* The standard prologue. */
void
prologue ()
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

void
epilogue ()
{
  printf ("\tmov\t%%eax, 0\n");
  printf ("\tleave\n");
  printf ("\tret\n");
}

/* Initialize global structures. */
void
init ()
{
  GetChar ();
  atexit (epilogue);
}

/* The main program. */
int
main (int argc, char *argv[])
{
  init ();
  prologue ();
  program ();
  return 0;
}
