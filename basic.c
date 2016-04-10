
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <err.h>

/* ------------------------------------------------------------------ */

void prologue ();
void epilogue ();

/* ------------------------------------------------------------------ */

char *KEYWORDS = "IF\x81" "ELSE\x82" "LET\x83" "GOTO\x84"
  "OR\x85" "AND\x86" "NOT\x87" "THEN\x88";

/* ------------------------------------------------------------------ */

#define IF   -127
#define ELSE -126
#define LET  -125
#define GOTO -124
#define OR   -123
#define AND  -122
#define NOT  -121
#define THEN -120

/* ------------------------------------------------------------------ */

int
is_addop (char c)
{
  return (c == '+' || c == '-' || c == '|');
}

/* ------------------------------------------------------------------ */

int
is_mulop (char c)
{
  return (c == '*' || c == '/' || c == '&');
}

/* ------------------------------------------------------------------ */

bool
is_relop (char c)
{
  return (c == '<' || c == '>' || c == '=');
}

/* ------------------------------------------------------------------ */

int 
is_op (char c)
{
  return (is_addop (c) || is_mulop (c));
}

/*
 * ERROR UNIT
 */

/* ------------------------------------------------------------------ */

void
syntax_error ()
{
  fprintf (stderr, "? Syntax Error\n");
  exit (EXIT_FAILURE);
}

/* ------------------------------------------------------------------ */

void
out_of_memory ()
{
  fprintf (stderr, "Out of memory\n");
  exit (EXIT_FAILURE);
}

/* ------------------------------------------------------------------ */

void
undefined (char *name)
{
  fprintf (stderr, "Undefined variable '%s'\n", name);
  exit (EXIT_FAILURE);
}


/* SYMBOL TABLE */

/* ------------------------------------------------------------------ */

struct symbol
{
  char *name;
  struct symbol *next;
};

/* ------------------------------------------------------------------ */
/* The symbol table. */

static struct symbol *symbol_table = NULL;
void allot (char *);

/* ------------------------------------------------------------------ */

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
	  char cur, term = *s;
	  *d = *s; /* Copy opening quote. */
	  ++d; /* Skip open quote in destination. */
	  ++s; /* Skip open quote in source. */
	  while (*s)
	    {
	      cur = *s; /* Copy next char. */
	      *d = cur;
	      ++d;
	      ++s;
	      if (cur == term)
		break; /* If end quote, break. */
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
		  if (toupper (s[i]) != k[i])
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

/* ------------------------------------------------------------------ */

void
Dump (char *s)
{
  char *p;
  for (p = s; *p; p++)
    fprintf (stderr, "\t%c\t$%hhx\t%d\n", *p, *p, *p);
}

/* ------------------------------------------------------------------ */

int Look;

/* ------------------------------------------------------------------ */
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

/* ------------------------------------------------------------------ */

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

/* ------------------------------------------------------------------ */

void
match (char c)
{
  if (Look != c)
    {
      switch (c)
        {
          case IF:
            fprintf (stderr, "Expected IF\n");
            break;
          case ELSE:
            fprintf (stderr, "Expected ELSE\n");
            break;
          case LET:
            fprintf (stderr, "Expected LET\n");
            break;
          case GOTO:
            fprintf (stderr, "Expected GOTO\n");
            break;
          case OR:
            fprintf (stderr, "Expected OR\n");
            break;
          case AND:
            fprintf (stderr, "Expected AND\n");
            break;
          case NOT:
            fprintf (stderr, "Expected NOT\n");
            break;
          case THEN:
            fprintf (stderr, "Expected THEN\n");
            break;
          default:
            fprintf (stderr, "Expected %c\n", c);
            break;
        }
      exit (EXIT_FAILURE);
    }
  GetChar ();
}

/* ------------------------------------------------------------------ */

void
Expected (char *what)
{
  printf ("Expected %s\n", what);
  exit (EXIT_FAILURE);
}

/* ------------------------------------------------------------------ */

char *
get_var ()
{
  char name[8];
  int len = 0;
  if (!isalpha (Look))
    syntax_error ();
  while (isalnum (Look))
    {
      name[len++] = toupper (Look);
      GetChar ();
    }
  name[len] = '\0';
  struct symbol *sp = lookup (name);
  return sp->name;
}

/* ------------------------------------------------------------------ */

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

/* ----------------------------------------------------------------- */



/* ----------------------------------------------------------------- */
/* CODE GENERATION */

/* ----------------------------------------------------------------- */
/* Load constant N into primary register. */

void
load_constant (int n)
{
  printf ("\tmov\t%%rax, %d\n", n);
}

/* ----------------------------------------------------------------- */
/* Load a variable into primary register. */

void
load_variable (char *name)
{
  printf ("\tmov\t%%rax, %s\n", name);
}

/* ----------------------------------------------------------------- */
/* Store primary value into variable.  */

void
store (char *name)
{
  printf ("\tmov\t%s, %%rax\n", name);
}

/* ----------------------------------------------------------------- */
/* Push primary register to stack. */

void
push ()
{
  printf ("\tpush\t%%rax\n");
}

/* ----------------------------------------------------------------- */
/* Divide top of stack by primary value. */

void
pop_div (void)
{
  printf ("\tmov\t%%rcx, %%rax\n");
  printf ("\tpop\t%%rax\n");
  printf ("\tmov\t%%rdx, %%rax\n");
  printf ("\tsar\t%%rdx, 63\n");
  printf ("\tidiv\t%%rcx\n");
}

/* ----------------------------------------------------------------- */
/* Multiply primary value by top of stack. */

void
pop_mul (void)
{
  printf ("\tpop\t%%rdx\n");
  printf ("\timul\t%%rax, %%rdx\n");
}

/* ----------------------------------------------------------------- */
/* Subtract primary from top of stack.  */

void
pop_sub (void)
{
  printf ("\tmov\t%%rdx, %%rax\n");
  printf ("\tpop\t%%rax\n");
  printf ("\tsub\t%%rax, %%rdx\n");
}

/* ----------------------------------------------------------------- */
/* Add top of stack to primary.  */

void
pop_add (void)
{
  printf ("\tpop\t%%rdx\n");
  printf ("\tadd\t%%rax, %%rdx\n");
}

/* ----------------------------------------------------------------- */
/* Logical AND top of stack with primary accumulator. */

void
pop_and ()
{
  printf ("\tpop\t%%rdx\n");
  printf ("\tand\t%%rax, %%rdx\n");
}

/* ----------------------------------------------------------------- */
/* Or top of stack with primary accumulator. */

void
pop_or ()
{
  printf ("\tpop\t%%rdx\n");
  printf ("\tor\t%%rax, %%rdx\n");
}

/* ----------------------------------------------------------------- */
/* Compare primary value with top of stack. */

void
pop_compare ()
{
  printf ("\tpop\t%%rdx\n");
  printf ("\tcmp\t%%rdx, %%rax\n");
}

/* ----------------------------------------------------------------- */
/* Sign Extend Boolean Value to RAX */

void
sign_extend_bool ()
{
  printf ("\tnegb\t%%al\n");
  printf ("\tmovsx\t%%rax, %%al\n");
}

/* ----------------------------------------------------------------- */
/* Load "true" if last comparison was equal. */

void
set_equal ()
{
  printf ("\tseteb\t%%al\n");
  sign_extend_bool ();
}

/* ----------------------------------------------------------------- */
/* Sets byte if last comparison was not equal. */

void
set_not_equal ()
{
  printf ("\tsetneb\t%%al\n");
  sign_extend_bool ();
}

/* ----------------------------------------------------------------- */
/* Sets byte if last comparison was less or equal. */

void
set_less_or_equal ()
{
  printf ("\tsetleb\t%%al\n");
  sign_extend_bool ();
}

/* ----------------------------------------------------------------- */
/* Sets byte if last comparison was less. */

void
set_less ()
{
  printf ("\tsetbb\t%%al\n");
  sign_extend_bool ();
}

/* ----------------------------------------------------------------- */

void
set_greater_or_equal ()
{
  printf ("\tsetgeb %%al\n");
  sign_extend_bool ();
}

/* ----------------------------------------------------------------- */

void
set_greater ()
{
  printf ("\tsetgb %%al\n");
  sign_extend_bool ();
}

/* ----------------------------------------------------------------- */
/* Allocate space for a variable. */

void
allot (char *name)
{
  printf ("\t.comm\t%s, 8\n", name);
}

/* ----------------------------------------------------------------- */
/* Unconditional jump to label. */

void
jump (int line_number)
{
  printf ("\tjmp\tL%d\n", line_number);
}

/* ----------------------------------------------------------------- */

void
negate ()
{
  printf ("\tnot\t%%rax\n");
}

/* ----------------------------------------------------------------- */

void
branch_if (int lineno)
{
  printf ("\ttest\t%%rax, 0\n");
  printf ("\tjnz\tL%d\n", lineno);
}



/* ----------------------------------------------------------------- */
/* PARSER */

/* ----------------------------------------------------------------- */

void add ();
void subtract ();
void multiply ();
void divide ();
void factor ();
void term ();
void expression ();
void bool_expression ();
void bool_term ();
void bool_factor ();
void or ();
void and ();
void not_factor ();
void relation ();

/* ----------------------------------------------------------------- */
/* Parse and translate a factor. */

void
term ()
{
  if (Look == '(')
    {
      match ('(');
      expression ();
      match (')');
    }
  else if (isalpha (Look))
    load_variable (get_var ());
  else if (isdigit (Look))
    load_constant (get_number ());
  else
    syntax_error ();
}

/* ----------------------------------------------------------------- */
/* Parse and translate an operator. */

void
operator (char op)
{
  switch (op)
    {
      case '+': pop_add (); break;
      case '-': pop_sub (); break;
      case '*': pop_mul (); break;
      case '/': pop_div (); break;
      default:
        errx (EXIT_FAILURE, "unknown operator %c", op);
        break;
    }
}

/* ----------------------------------------------------------------- */
/* Return operator precendence. */

int
operator_precedence (char op)
{
  switch (op)
    {
    case '|':
      return 1;
    case '&':
      return 2;
    case '+':
    case '-':
      return 3;
    case '*':
    case '/':
      return 4;
    default:
      return 0;
    }
}

/* ----------------------------------------------------------------- */
/* Relative operator precedence. */

int
precedence_compare (char op1, char op2)
{
  return operator_precedence (op1) - operator_precedence (op2);
}

/* ----------------------------------------------------------------- */
/* Parse and translate a mathematical expression.  */

void
expression (void)
{
  char stack [16];
  const int stack_sz = sizeof (stack) / sizeof (stack[0]);
  int i = 0;

  term ();

  while (is_op (Look))
    {
      char op = Look;
      GetChar ();
      while (i > 0 && precedence_compare (op, stack[i-1]) <= 0)
        {
          operator (stack [--i]);
        }
      if (i >= stack_sz)
        {
          errx (EXIT_FAILURE, "operator stack overflow");
        }
      stack [i++] = op;
      push ();
      term ();
    }

  while (i > 0)
    {
      operator (stack [--i]);
    }
}

/* ----------------------------------------------------------------- */
/* Parse and translate AND of two terms.  */

void
and ()
{
  match (AND);
  not_factor ();
  pop_and ();
}

/* ----------------------------------------------------------------- */

void
or ()
{
  match (OR);
  bool_term ();
  pop_or ();
}

/* ----------------------------------------------------------------- */

void
equals ()
{
  match ('=');
  expression ();
  pop_compare ();
  set_equal ();
}

/* ----------------------------------------------------------------- */

void
less_or_equal ()
{
  match ('=');
  expression ();
  pop_compare ();
  set_less_or_equal ();
}

/* ------------------------------------------------------------------ */

void
greater_or_equal ()
{
  match ('=');
  expression ();
  pop_compare ();
  set_greater_or_equal ();
}

/* ------------------------------------------------------------------ */
/* Recognize and Translate a Relational "Not Equals" */

void
not_equal ()
{
  match ('>');
  expression ();
  pop_compare ();
  set_not_equal ();
}

/* ------------------------------------------------------------------ */
/* Recognize and Translate a Relational "Less Than" */

void
less_than ()
{
  match ('<');
  switch (Look)
    {
      case '=':
        less_or_equal ();
        break;
      case '>':
        not_equal ();
        break;
      default:
        expression ();
        pop_compare ();
        set_less ();
        break;
    }
}

/* ----------------------------------------------------------------- */

void
greater_than ()
{
  match ('>');
  switch (Look)
    {
      case '=':
        greater_or_equal ();
        break;
      case '>':
        not_equal ();
        break;
      default:
        expression ();
        pop_compare ();
        set_greater ();
        break;
    }
}

/* ----------------------------------------------------------------- */
/* Parse and translate a relation.  */

void
relation ()
{
  expression ();
  if (is_relop (Look))
    {
      push ();
      switch (Look)
        {
          case '=':
            equals ();
            break;
          case '<':
            less_than ();
            break;
          case '>':
            greater_than ();
            break;
        }
    }
}


/* ----------------------------------------------------------------- */

void
bool_term ()
{
  not_factor ();
  while (Look == AND)
    {
      push ();
      and ();
    }
}

/* ----------------------------------------------------------------- */
/* Parse and translate a condition. */
 
void
bool_expression ()
{
  bool_term ();
  while (Look == OR)
    {
      push ();
      or ();
    }
}

/* ----------------------------------------------------------------- */

void
not_factor ()
{
  if (Look == NOT)
    {
      match (NOT);
      relation ();
      negate ();
    }
  else
    {
      relation ();
    }
}

/* ----------------------------------------------------------------- */
/* Parse and translate an assignment statement.  */

void
assignment ()
{
  if (Look == LET)
    match (LET);
  char *var = get_var ();
  match ('=');
  expression ();
  store (var);
}

/* ----------------------------------------------------------------- */
/* Parse and translate a label. */

void
label ()
{
  int n = get_number ();
  printf ("L%d:\n", n);
}

/* ----------------------------------------------------------------- */
/* Parse and translate a goto statement. */

void
goto_stmt ()
{
  if (Look == GOTO)
    match (GOTO);
  int lineno = get_number ();
  jump (lineno);
}

/* ----------------------------------------------------------------- */
/* Parse and translate an if statement. */

void
if_stmt ()
{
  match (IF);
  bool_expression ();
  match (THEN);
  match (GOTO);
  int lineno = get_number ();
  branch_if (lineno);
}

/* ----------------------------------------------------------------- */
/* Parse and translate a statement. */

void
statement ()
{
  if (isdigit (Look))
    label ();
  switch (Look)
    {
    case IF:
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

/* ----------------------------------------------------------------- */
/* Parse and translate a program.  */

void
program ()
{
  while (Look != EOF)
    {
      statement ();
    }
}

/* ----------------------------------------------------------------- */
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

/* ----------------------------------------------------------------- */

void
epilogue ()
{
  printf ("\tmov\t%%eax, 0\n");
  printf ("\tleave\n");
  printf ("\tret\n");
}

/* ----------------------------------------------------------------- */
/* Initialize global structures. */

void
init ()
{
  GetChar ();
  atexit (epilogue);
}

/* ----------------------------------------------------------------- */
/* The main program. */

int
main (int argc, char *argv[])
{
  init ();
  //prologue ();
  //program ();
  expression();
  return 0;
}

