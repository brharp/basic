

void enter()
{
	emit("push %s", EBP);
	emit("mov %s, %s", EBP, ESP);
}


/* Parse and translate a (FOR) loop. */
int forc(void)
{
	enter();
	letc();
	toc();
	stepc();
	g = gen();
	emit("call %s%d", LABEL, g);
	printf("%s%d:\n", LABEL, g);
}

	/* Match FOR */
	if (sym == FOR)
		sym = get();
	else
		mark("FOR ?");

	/* Match variable name */
	if (sym == ID) {
		strcpy(x, id);
		getsym();
	}

	if (sym == EQ)
		sym = get();
	else
		mark("= ?");

	expr();
	emit("mov %s, %s", x, rax);

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
	emit("call G%d", label);
	putlabel(L);
}

/* NEXT handler */
void nextc(int top)
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

