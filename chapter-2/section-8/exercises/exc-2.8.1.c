typedef struct {
  /* variable(s) */
  Expr expr_01;
  Expr expr_02;
  Expr expr_03;

  Stmt stmt;

  /* function(s) */
  void (*For)(Expr x, Stmt y);
  void (*gen)(void);
} For;

void constr_For(For *for_stmt, Expr expr1, Expr expr2, Expr expr3, Stmt stmt) {
  /* constructor */
  for_stmt->expr_01 = expr1;
  for_stmt->expr_02 = expr2;
  for_stmt->expr_03 = expr3;

  for_stmt->stmt = stmt;
}

void gen_For(For *for_stmt) {
  for_stmt->expr_01.expr_gen();

  // ...
}
