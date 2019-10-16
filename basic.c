/* BASIC */

#define _GNU_SOURCE 1

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>

#include "token.h"
#include "out.h"

#define TOKENMAX 80
#define INTSIZE 8

#define countof(X) (sizeof(X) / sizeof(X[0]))

#define TO 'o'
#define STEP 's'
#define NEXT 'n'

/* Function declarations */

void getname(void);
void getnum(void);
void getop(void);
void getstr(void);
void init(void);
char *intern(char *);
int main(int argc, char *argv[]);
void next(void);
void nextchar(void);
void skipspace(void);
void syntaxerror(void);
/*int lookup (const char *table[], char *key, int n);*/
int kwsearch(void);
void scan(void);

/* Constants */

char *keywords[] = {"LET", "IF", "THEN", "GOTO", "FOR", "TO",
                    "STEP", "NEXT", "PRINT", "$TRING", "DIM", "INPUT"};

const char keycodes[] = "xlitgfosnp$dr";

/* Variables */

int lookahead;
int tokentype;
char tokentext[TOKENMAX + 1];
int tokenlength;
int ln; /* Line number */
extern enum keyword sym;
extern float val;

/* Registers */

const char *rax = "%rax", *rbx = "%rbx", *rcx = "%rcx", *rdx = "%rdx",
           *al = "%al", *rbp = "%rbp", *rsp = "%rsp";

/* Code Templates */

#define ALOAD(ARRAY, INDEX) "\tmov\t%%rax, " ARRAY "[%%rip+" INDEX "]\n"
#define LOAD(X) "\tmov\t%%rax, " X "\n"
#define JUMP(LINENO) "\tjmp\tL" LINENO "\n"

/* Code generators */

void emit(const char *fmt, ...)
{
  va_list ap;
  putchar('\t');
  va_start(ap, fmt);
  vprintf(fmt, ap);
  va_end(ap);
  putchar('\n');
}

void aload(const char *array, const int index)
{
  printf(ALOAD("%1$s", "%2$d"), array, index);
}

void load(const char *x)
{
  printf(LOAD("%1$s"), x);
}

void die(const char *msg)
{
  fprintf(stderr, "?%s IN %d\n", msg, ln);
  exit(EXIT_FAILURE);
}

void redimerror(void)
{
  die("REDIM'D ARRAY");
}

void syntaxerror(void)
{
  die("SYNTAX ERROR");
}

int lookup(char *table[], char *key, int n)
{
  int i = n;
  while (i--)
    if (strcmp(table[i], key) == 0)
      break;
  return i;
}

void linenumber(void)
{
  if (sym != LINE)
    mark("Line number?");
  else
  {
    /* Generate a line number label. */
    printf("L%s:\n", nb);
    sym = getsym();
  }
}

void factor(void)
{
  if (sym == IDENT)
  {
    sym = getsym();
    if (sym == '(')
    {
      sym = getsym();
      if (sym == NUMBER)
      {
        emit("mov %s, %s[%s+%d]", rax, id, rip, val * INTSIZE);
        sym = getsym();
      }
      else
        mark("Index?");

      if (sym == ')')
        sym = getsym();
      else
        mark(")?");
    }
    else
    {
      emit("mov %s, %s", rax, id);
      sym = getsym();
    }
  }
  else if (sym == NUMBER)
  {
    emit("mov %s, %f", rax, val);
    sym = getsym();
  }
}

/* Translate a term. */
void term(void)
{
  factor();
  while (sym == '*' || sym == '/')
  {
    emit("push %s", rax);
    switch (sym)
    {
    case '*':
      emit("pop %s", rdx);
      emit("imul %s, %s", rax, rdx);
      break;
    case '/':
      emit("mov %s, %s", rcx, rax);
      emit("pop %s", rax);
      emit("mov %s, %s", rdx, rax);
      emit("sar %s, %d", rdx, 63);
      emit("idiv %s", rcx);
      break;
    }
    sym = getsym();
  }
}

void expression(void)
{
  term();
  while (sym == '+' || sym == '-')
  {
    emit("push %s", rax);
    switch (sym)
    {
    case '+':
      sym = getsym();
      term();
      emit("pop %s", rdx);
      emit("add %s, %s", rax, rdx);
      break;
    case '-':
      sym = getsym();
      term();
      emit("mov %s, %s", rdx, rax);
      emit("pop %s", rax);
      emit("sub %s, %s", rax, rdx);
      break;
    }
  }
}

char *symboltable[128];
int symbolcount;

/* Allocate space for a variable. */
void allot(char *name, int size)
{
  emit(".comm %s, %d", name, size);
}

/* Add name to symbol table. */
char *
intern(char *name)
{
  int i = symbolcount;
  /* Define symbol. */
  if (i >= countof(symboltable))
    die("symbol table full");
  /* Add entry to table. */
  symboltable[i] = strdup(name);
  /* Increment symbol table counter. */
  ++symbolcount;
  return symboltable[i];
}

void dim(void)
{
  if (sym == DIM)
    sym = getsym();

  if (sym != IDENT)
    syntaxerror();

  int i = lookup(symboltable, id, symbolcount);

  if (i > -1)
    redimerror();

  char *x = intern(id);
  sym = getsym();

  if (sym == '(')
    sym = getsym();
  else
    mark("(?");

  if (sym != NUMBER)
    syntaxerror();

  int n = (int)val;
  sym = getsym();

  if (sym == ')')
    sym = getsym();
  else
    mark(")?");

  allot(x, n * INTSIZE);
}

char *
identifier(void)
{
  /* Syntax check. */
  if (sym != IDENT)
    syntaxerror();
  /* Search for variable in symbol table. */
  int i = lookup(symboltable, id, symbolcount);
  /* If not defined, insert into table. */
  if (i < 0)
  {
    i = symbolcount;
    /* Allocate space for variable. */
    emit(".comm %s, %d\n", id, INTSIZE);
    /* Define symbol. */
    if (i >= countof(symboltable))
      die("symbol table full");
    /* Add entry to table. */
    symboltable[i] = strdup(id);
    /* Increment symbol table counter. */
    ++symbolcount;
  }
  /* Match variable. */
  sym = getsym();
  return symboltable[i];
}

void assignment(void)
{
  char *name;

  /* Match (optional) "LET" keyword. */
  if (sym == LET)
    sym = getsym();

  identifier();
  name = strdup(id);
  if (sym == '(')
  {
    sym = getsym();
    expression();
    emit("push %s", rax);
    if (sym == ')')
      sym = getsym();
    if (sym == '=')
      sym = getsym();
    expression();
    emit("pop %s", rbx);
    emit("mov QWORD PTR %s[%%rip+%d*%s], %s", name, INTSIZE, rbx, rax);
  }
  else
  {
    if (sym == '=')
      sym = getsym();
    expression();
    emit("mov %s, %s", name, rax);
  }
}

void statement(void);

/* Parse and translate a conditional (IF) statement. */
void conditional(void)
{
  /* Conditional jump label counter. */
  static int j = 100;
  /* Match the "IF" token. */
  if (match(IF))
    mark("IF?");
  /* Match the first expression. */
  expression();
  /* Push first value on the stack. */
  emit("push %s", rax);
  /* Match second expression in comparison. */
  expression();
  /* Pop first value into %rdx. */
  emit("pop %s", rdx);
  /* Compare expression values. */
  emit("cmp %s, %s", rdx, rax);
  /* If the comparison fails, jump over the consequent. */
  if (sym == LT)
    emit("jnl J%d", j);
  else if (sym == LTE)
    emit("jnle J%d", j);
  else if (sym == GT)
    emit("jng J%d", j);
  else if (sym == GTE)
    emit("jnge J%d", j);
  else if (sym == EQ)
    emit("jne J%d", j);
  else if (sym == NEQ)
    emit("jz J%d", j);
  else
    syntaxerror();
  /* Match "THEN" */
  if (sym == THEN)
    sym = getsym();
  else
    mark("THEN?");
  /* Compile the consequent; the statement to execute if the 
         condition is true. */
  branch();
  /* Target label of conditional jump instruction. */
  emit("J%d:", j++);
  /* TODO: Return label. */
}

/* Parse and translate a branch (aka GOTO). */
void branch(void)
{
  /* Match "GOTO" token. */
  if (sym == GOTO)
    sym = getsym();
  /* Convert line number token to int. */
  if (sym != NUMBER)
    syntaxerror();
  /* Jump to line number label. */
  emit("jmp L%d", (int) val);
  sym = getsym();
}

const char *limit = "QWORD PTR [%rbp-8]";
const char *step = "QWORD PTR [%rbp-16]";
const char *dir = "QWORD PTR [%rbp-24]";

/* Parse and translate a (FOR) loop. */
int For(void)
{
  static int counter = 0;

  int label = counter;
  counter += 2;

  /* Match FOR */
  if (sym == FOR)
    sym = get();
  else
    mark("FOR ?");
  char *id = identifier();
  if (sym == EQ)
    sym = get();
  else
    mark("= ?");
  expression();
  emit("mov %s, %s", id, rax);

  /* Match LIMIT */
  if (sym == TO)
    sym = get();
  else
    mark("TO ?");
  expression();
  emit("push %s", rax);

  /* Match INCREMENT */
  if (sym == STEP)
  {
    sym = get();
    expression();
    emit("push %s", rax);
  }
  else
    emit("push %d", 1);

  /* Push stack pointer */
  emit("push %s", rbp);
  emit("mov %s, %s", rbp, rsp);

  /* Start of loop body */
  printf("G%d:\n", label);

  return label;
}

/* NEXT handler */
void next(int top)
{
  /* Match NEXT */
  match('n');

  /* Parse variable 
     if (tokentype != 'x') || strcmp (id, tokentext) != 0)
     syntaxerror (); */

  //match ('x');
  char *id = identifier();

  emit("mov %s, %s", rax, id);        // Load counter
  emit("push %s", rax);               // Save original value
  emit("add %s, %s", rax, step); // Add loop increment
  emit("mov %s, %s", id, rax);        // Update counter
  emit("pop %s", rdx);                // Pop old value of counter
  emit("sub %s, %s", rax, limit);     // Test if counter has crossed
  emit("sub %s, %s", rdx, limit);     //  boundary between LIMIT
  emit("imul %s, %s", rax, rdx);      //  and LIMIT - 1
  emit("jge %s", label(top));

  /* Clear stack */
  emit("mov %s, %s", rsp, rbp);
  emit("pop %s", rbp);
}

/* Match a variable reference. */
void var(void)
{
  char *x = identifier();
  if (tokentype == '(')
  {
    match('(');
    int i = atoi(tokentext);
    match('#');
    match(')');
    printf("\tlea\t%%rax, %s[%%rip+%d]\n", x, i * INTSIZE);
  }
  else
  {
    printf("\tlea\t%%rax, %s\n", x);
  }
}

/* Input variable value. */
void input(void)
{
  /* Match INPUT token */
  match('r');
  printf("\tmov\t%%rax, 0\n");     /* read(2) */
  printf("\tmov\t%%rdi, 0\n");     /* file descriptor = 0 */
  printf("\tsub\t%%rsp, 2\n");     /* reserve space on stack */
  printf("\tmov\t%%rsi, %%rsp\n"); /*  pointer to char (top of stack) */
  printf("\tmov\t%%rdx, 1\n");     /* read 1 char */
  printf("\tsyscall\n");
  printf("\txor\t%%rbx, %%rbx\n"); /* clear rbx */
  printf("\tpop\t%%dx\n");         /* Pop character read */
  printf("\tmov\t%%bl, %%dl\n");
  var();
  printf("\tmov\t[%%rax], %%rbx\n"); /* store in variable */
}

/* Print variable value. */
void print(void)
{
  static int counter = 0;
  match('p');
  /* Print a literal string. */
  if (tokentype == '$')
  {
    int label = counter++;
    printf("\t.section .rodata\n");
    printf("STR%d:\n", label);
    printf("\t.string\t\"%s\"\n", tokentext);
    printf("\t.section .text\n");
    printf("\tmov\t%%rax, 1\n");                         /* write(2) */
    printf("\tmov\t%%rdi, 1\n");                         /* file descriptor 1 */
    printf("\tmov\t%%rsi, OFFSET FLAT: STR%d\n", label); /* ptr to char */
    printf("\tmov\t%%rdx, %d\n", tokenlength);           /* length of string */
    printf("\tsyscall\n");
    printf("\tcall\tnewline\n");
    match('$');
  }
  /* Print an expression. */
  else
  {
    expression();
    printf("\tcall\tprint\n");
  }
}

#define MAXSTACK 16

/* Parse and translate a statement. */
void statement(void)
{
  static int labels[MAXSTACK];
  static int sp = 0;

  /* Statements may begin with a line number. */
  if (tokentype == '#')
    /* Match line number. */
    linenumber();
  /* Switch on first token. */
  switch (tokentype)
  {
  /* GOTO  */ case 'g':
    branch();
    break;
  /* IF    */ case 'i':
    conditional();
    break;
    /* FOR   */
  case 'f':
    if (sp > MAXSTACK)
      die("stack overflow in parser");
    labels[sp++] = loop();
    break;
    /* NEXT  */
  case 'n':
    if (sp < 1)
      die("stack underflow in parser");
    fornext(labels[--sp]);
    break;
  /* PRINT */ case 'p':
    print();
    break;
  /* INPUT */ case 'r':
    input();
    break;
  /* DIM   */ case 'd':
    dim();
    break;
  /* LET   */ default:
    assignment();
    break;
  }
}

/* Parse and translate a block of statements. */
void block(void)
{
  while (lookahead != EOF)
  {
    statement();
    if (tokentype == 'n')
      break;
  }
}

/* Standard prologue. */
void prologue(void)
{
  printf("\t.intel_syntax\n");
  printf("\t.globl main\n");
  printf("main:\n");
  printf("\tpush\t%%rbp\n");
  printf("\tmov\t%%rbp, %%rsp\n");
  printf("\tsub\t%%rsp, 16\n");
  printf("\tmov\tDWORD PTR [%%rbp-4], %%edi\n");
  printf("\tmov\tQWORD PTR [%%rbp-16], %%rsi\n");
}

/* Standard epilogue. */
void epilogue(void)
{
  printf("\tmov\t%%eax, 0\n");
  printf("\tleave\n");
  printf("\tret\n");
}

char *
match_keyword(char *first, char *keyword)
{
  if (*keyword == '\0')
    return first;
  else if (*first == *keyword)
    return match_keyword(first + 1, keyword + 1);
  else
    return NULL;
}

void tokenize(void)
{
  char line[80];
  char *input = fgets(line, 80, stdin);
  int count = countof(keywords);
  while (*input)
  {
    int i;
    for (i = 0; i < count; i++)
    {
      char *p;
      if ((p = match_keyword(input, keywords[i])))
      {
        printf("TOKEN: %s\n", keywords[i]);
        input = p;
      }
    }
    if (i == count)
    {
      printf("CHAR: %c\n", *input++);
    }
  }
}

/* Main program. */
int main(int argc, char *argv[])
{
  //for (;;)
  //tokenize();
  init();
  prologue();
  block();
  epilogue();
  return 0;
}
