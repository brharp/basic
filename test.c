
int x = 0;

int
main (int argc, char *argv[])
{
L100:
  x = x + 1;
  if (x < 10) goto L100;
}

