#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

char linebuffer[72], *lineptr;
int  currentline;

struct programline {
  struct programline *next;
  int linenumber;
  char *str;
} *program;

char
inputchar()
{
  return fgetc(stdin);
}

char
nextchar()
{
  char c;
  while ((c=*lineptr)==' ')
    ++lineptr;
  return c;
}

newline()
{
  fputc('\n', stdout);
}

inputline()
{
  char *p=linebuffer, c;
  while ((c=inputchar()) != '\n')
    if (isprint(c))
      *p++ = c;
  *p = 0;
  lineptr=linebuffer;
  newline();
}

int
linenumber()
{
  int linenumber = -1, n;
  sscanf(linebuffer, "%d%n", &linenumber, &n);
  lineptr = linebuffer+n; /* skip number */
  return linenumber;
}

struct programline *
programlinealloc()
{
  struct programline *p;
  if ((p = malloc(sizeof(*p))) == NULL) {
    fprintf(stderr, "programlinealloc: out of memory\n");
    exit(1);
  }
  return p;
}

storeprogramline(int linenumber)
{
  struct programline *line, **p=&program;
  line = programlinealloc();
  line->next = 0;
  line->linenumber = linenumber;
  line->str = strdup(lineptr);
  while (*p!=NULL && (*p)->linenumber < linenumber) {
    p = &((*p)->next);
  }
  line->next = (*p); (*p)=line;
}

list()
{
  struct programline *p=program;
  while (p!=NULL) {
    printf("%d %s\n", p->linenumber, p->str);
    p=p->next;
  }
}

exec()
{
}

main ()
{
  for (;;) {
    printf("OK\n"); /* prompt */
    currentline = -1; /* set immediate mode */
    do { /* get nonblank line */
      inputline();
    } while (nextchar() == 0);
    int n=linenumber();
    //tokenize();
    if (n<0)
      list();
    else
      storeprogramline(n);
  }
}

