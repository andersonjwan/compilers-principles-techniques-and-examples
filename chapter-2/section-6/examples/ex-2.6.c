#include <stdio.h>

/* terminal(s) */
enum terminals {NUM = 256, ID, TRUE, FALSE};

/* global(s) */
static int line = 1;
static char peek = ' '; // blank-space initially

/* data structure(s) */
typedef struct Tokens {
  int tag;      // used for parsing decisions (i.e. terminal)
  int line;  // line number token on

  int value;    // integer values
  char *lexeme; // reserved words and/or identifiers
} Token;

Token words[1000];        // string table for keywords and identifiers
static int next_word = 0; // next available slot

void reserve(Token t) {
  words[next_word] = t;
  ++next_word;
}

Token scan(void) {
  /* iterate until non-whitespace / tab / newline is met */
  for( ; ; peek = (char) getchar()) {
    if(peek == ' ' || peek == '\t') {
      continue;        // ignore whitespace and tab(s)
    }
    else if(peek == '\n') {
      line = line + 1; // next line
    }
    else {
      break;
    }
  }

  /* non-whitespace / tab / newline met */
  if(peek >= 48 && peek <= 57) {
    /* peek is digit */

    int v = 0;
    do {
      v = v * 10 + (peek - 48);      // v * 10 + integer value of peek
      peek = getchar();
    }
    while(peek >= 48 && peek <= 57); // while peek is a digit

    /* create and return new token */
    Token t;
    t.tag = NUM;
    t.line = line;
    t.value = v;

    return t;
  }

  if((peek >= 65 && peek <= 90) || (peek >= 97 && peek <= 122)) {
    /* peek is a letter (uppercase OR lowercase) */
    int size = 2, next_char = 0;
    char *buffer = (char *) malloc(sizeof(char) * size);

    if(buffer == NULL) {
      fprintf(stderr, "Memory Allocation Error: Unable to allocate memory.\n");
      exit(1);
    }

    do {
      /* allocate more memory if needed */
      if(next_char >= size) {
        char *temp_buffer = realloc(buffer, size + 2);
        size = size + 1;

        if(temp_buffer != NULL) {
          fprintf(stderr, "Memory Allocation Error: Unable to reallocate memory.\n");
          exit(2);
        }

        buffer = temp_buffer;
      }

      /* store character */
      buffer[next_char] = peek;
      ++next_char;

      peek = getchar();
    }
    while((peek >= 65 && peek <= 90) || (peek >= 97 && peek <= 122));

    /* append null terminating character */
    buffer[next_char] = '\0';

    /* create and return new token */
    Token t;
    t.tag = ID;
    t.line = line;
    t.lexeme = buffer;

  }
}

int main(void) {
  /* setup reserved words */
  Token keyword;

  keyword.tag = TRUE;
  keyword.lexeme = "true";
  reserve(keyword);

  keyword.tag = FALSE;
  keyword.lexeme = "false";
  reserve(keyword);

  return 0;
}
