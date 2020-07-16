#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/* global variable(s) */
static int lookahead;

void match(int token) {
  if(lookahead == token) {
    lookahead = getchar(); // next token
  }
  else {
    fprintf(stderr, "Syntax Error : %c\n", lookahead);
    exit(1);
  }
}

void term(void) {
  if(lookahead >= 48 && lookahead <= 57) {
    /* lookahead is digit */
    printf("%c", lookahead);

    match(lookahead);
  }
  else {
    fprintf(stderr, "Syntax Error : %c\n", lookahead);
    exit(1);
  }
}

void expr(void) {
  term();

  /* rest() function, iteratively */
  while(true) {
    if(lookahead == '+') {
      match('+');
      term();

      printf("+");
    }
    else if(lookahead == '-') {
      match('-');
      term();

      printf("-");
    }
    else {
      /* epsilon, exit rest function */
      return;
    }
  }
}

int main(void)
{
  lookahead = getchar();
  expr();

  return 0;
}
