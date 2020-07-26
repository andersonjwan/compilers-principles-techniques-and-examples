#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hash.h"

/* terminal(s) */
enum terminals {NUM = 256, ID, TRUE, FALSE};

typedef struct Tokens {
  int tag;      // used for parsing decisions (i.e. terminal)
  int line;     // line number token on

  int value;    // integer values
  char *lexeme; // reserved words and/or identifiers
} Token;

/* global(s) */
static int line = 1;
static char peek = ' '; // blank-space initially

static hash_table *words;

void reserve(Token *t) {
  hash_insert(words, t->lexeme, t);
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

  /* NUM */
  if(peek >= 48 && peek <= 57) {
    /* peek is digit */
    int v = 0;
    do {
      v = v * 10 + (peek - 48);      // v * 10 + integer value of peek
      peek = getchar();
    }
    while(peek >= 48 && peek <= 57); // while peek is a digit

    printf("VALUE: %d\n", v);
    /* create and return new token */
    Token t;
    t.tag = NUM;
    t.line = line;
    t.value = v;
    t.lexeme = NULL;

    return t;
  }

  /* ID */
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
      if(size <= next_char) {
        size = size + 2;
        char *temp_buffer = realloc(buffer, size);

        if(temp_buffer == NULL) {
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

    /* check existence of token */
    Token *t = (Token *) hash_search(words, buffer);

    if(t != NULL) {
      return *t;
    }

    /* create and return new token */
    t = (Token *) malloc(sizeof(Token));

    t->tag = ID;
    t->line = line;
    t->lexeme = buffer;

    hash_insert(words, buffer, t);
    return *t;
  }

  /* create and return current character as Token */
  Token t;

  t.tag = peek;
  t.line = line;
  t.value = 0;
  t.lexeme = NULL;

  peek = ' '; // reset peek to blank-space

  return t;
}

void garbage_collector(void *value) {
  Token *tmp;
  tmp = (Token *) value;

  free(tmp->lexeme);
  free(tmp);
}

int main(void) {
  words = new_hash_table(garbage_collector);

  /* setup reserved words */
  Token *keyword_1, *keyword_2;
  keyword_1 = (Token *) malloc(sizeof(Token));
  keyword_2 = (Token *) malloc(sizeof(Token));

  keyword_1->tag = TRUE;
  keyword_1->lexeme = (char *) malloc(sizeof(char) * 5);
  strcpy(keyword_1->lexeme, "true");

  reserve(keyword_1);

  keyword_2->tag = FALSE;
  keyword_2->lexeme = (char *) malloc(sizeof(char) * 6);
  strcpy(keyword_2->lexeme, "false");

  reserve(keyword_2);

  Token t;
  while(1) {
    t = scan();
    hash_print(words);

    if(t.lexeme != NULL ) {
      if(strcmp(t.lexeme, "EXIT") == 0) {
        break;
      }
    }
  }

  del_hash_table(words);

  return 0;
}
