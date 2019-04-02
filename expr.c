void expression (void)
{
  term ();
  while (tokentype == '+' || tokentype == '-')
    {
      printf ("\tpush\t%%rax\n");//put(push, rax)
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

