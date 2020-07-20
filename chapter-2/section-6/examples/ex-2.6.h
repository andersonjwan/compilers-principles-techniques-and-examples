#ifndef EX_2_6_H
#define EX_2_6_H
/* data structure(s) */
typedef struct Tokens {
  int tag;      // used for parsing decisions (i.e. terminal)
  int line;  // line number token on

  int value;    // integer values
  char *lexeme; // reserved words and/or identifiers
} Token;

#endif
