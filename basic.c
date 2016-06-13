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

const char *keywords[] = { "LET", "IF" };
const char keycodes[] = "xli"; 

/* Variables */

int lookahead;
int tokentype;
char tokentext[TOKENMAX+1];
int tokenlength;

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
        syntaxerror ();

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

int main (int argc, char *argv[])
{
  init ();
  while (lookahead != EOF)
    {
      scan ();
      printf ("%c\t%s\n", tokentype, tokentext);
      next ();
    }
  return 0;
}

