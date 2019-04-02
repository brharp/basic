
struct Stmt_ {
	enum { For, Goto, Next, Print } Kind;
	union {
		struct { } For;
		struct { } Goto;
		struct { } Next;
		struct { } Print;
	} u;
};

/* constructor for ForStmt */
Stmt ForStmt(var, start, end, step) {
	Stmt o = checked_malloc (sizeof *o);
	o->Kind = For; o->u.For.Var = var; o->u.For.Start = start;
	return o;
}
