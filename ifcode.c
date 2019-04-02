/* ifcode - generate code for if statement */
int ifcode() {
	const int L = genlab(1);
	expr();
	put(mov, rax, rbx);
	op = relop();
	expr();
	put(cmp, rax, rbx);
	put(jcc, cc(op), L);
	return L;
}
/* cc - return condition code for relop */
static const char *cc (const char *s) {
	int i = 0;
	const char *c[] = { "l","e","le","g","ne","ge","mp" };
	for (char *p = s; *p; p++) i |= 1 << ((*p) - '<');
	return c[i];
}
