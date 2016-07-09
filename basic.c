/* BASIC */

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
int lookup (const char *table[], char *key, int n);
int kwsearch (void);
void scan (void);

/* Constants */

const char *keywords[] = { "LET", "IF", "THEN", "GOTO" };
const char keycodes[] = "xlitg"; 

/* Variables */

int  lookahead;
int  tokentype;
char tokentext[TOKENMAX+1];
int  tokenlength;

void syntaxerror (void)
{
  fprintf (stderr, "\nSyntax error\n");
  exit (EXIT_FAILURE);
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


int lookup (const char *table[], char *key, int n)
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

void label (void)
{
  printf ("L%s:\n", tokentext);
  next ();
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

void assignment (void)
{
  if (tokentype == 'l')
    match ('l');
  char *var = strdup (tokentext);
  match ('x');
  match ('=');
  expression ();
  printf ("\tmov\t%s, %%rax\n", var); 
  free (var);
}

void statement (void);

void conditional (void)
{
  static int j = 100;
  match ('i');
  expression ();
  printf ("\tpush\t%%rax\n");
  if (tokentype == '<' || tokentype == '>' || tokentype == '=')
    {
      char *cmp;
      switch (tokentype)
        {
          case '<':
            match ('<');
            switch (tokentype)
              {
                case '>': match ('>'); cmp = "jz"; break;
                case '=': match ('='); cmp = "jnle"; break;
                default: cmp = "jnl"; break;
              }
            break;
          case '>':
            match ('>');
            switch (tokentype)
              {
                case '<': match ('<'); cmp = "jz"; break;
                case '=': match ('='); cmp = "jnge"; break;
                default: cmp = "jng"; break;
              }
            break;
          case '=':
            match ('=');
            switch (tokentype)
              {
                case '<': match ('<'); cmp = "jnle"; break;
                case '>': match ('>'); cmp = "jnge"; break;
                default: cmp = "jne"; break;
              }
            break;
        }
      expression ();
      printf ("\tpop\t%%rdx\n");
      printf ("\tcmp\t%%rax, %%rdx\n");
      printf ("\t%s\tJ%d\n", cmp, j);
      match ('t');
      statement ();
      printf ("J%d:\n", j++);
    }
}

void branch (void)
{
  match ('g'); // GOTO
  int lineno = atoi (tokentext);
  match ('#'); // <LINE NUM>
  printf ("\tjmp\tL%d\n", lineno);
}

void statement (void)
{
  if (tokentype == '#')
    label ();
  scan ();
  switch (tokentype)
    {
      case 'g':
        branch ();
        break;
      case 'i':
        conditional ();
        break;
      default:
        assignment ();
        break;
    }
}

int main (int argc, char *argv[])
{
  init ();
  while (lookahead != EOF)
    {
      statement ();
    }
  return 0;
}

