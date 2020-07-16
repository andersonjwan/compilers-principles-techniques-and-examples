#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

char *words[1000];        // string table for keywords and identifiers
int next_word = 0;        // next available word

Token *tokens[1000];       // token table for all tokens
int next_token = 0;       // next available token

void reserve(Token *t, char *string) {
  words[next_word] = string;
  ++next_word;

  tokens[next_token] = t;
  ++next_token;
}

Token * get(char *string) {
  for(int i = 0; i < next_word; ++i) {
    if(strcmp(string, words[i]) == 0) {
      /* lexeme exist in table */
      return tokens[next_token];
    }
  }

  return NULL;
}

void put(char *string, Token *t) {
  words[next_word] = string;
  ++next_word;

  tokens[next_token] = t;
  ++next_token;
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
    Token *t = (Token *) malloc(sizeof(Token));
    t->tag = NUM;
    t->line = line;
    t->value = v;

    return *t;
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
    Token *t = (Token *) get(buffer);

    if(t != NULL) {
      return *t;
    }

    /* create and return new token */
    t = (Token *) malloc(sizeof(Token));

    t->tag = ID;
    t->line = line;
    t->lexeme = buffer;

    put(buffer, t);
    return *t;
  }

  Token t;
  t.tag = peek;
  peek = ' '; // reset peek to blank-space

  return t;
}

int main(void) {
  /* setup reserved words */
  Token keyword_1, keyword_2;

  keyword_1.tag = TRUE;
  keyword_1.lexeme = "true";
  reserve(&keyword_1, keyword_1.lexeme);

  keyword_2.tag = FALSE;
  keyword_2.lexeme = "false";
  reserve(&keyword_2, keyword_2.lexeme);

  while(1) {
    scan();

    for(int i = 0; i < next_token; ++i) {
      /* print Token tag */
      printf("Token Tag: ");
      if(tokens[i]->tag == NUM) {
        printf("NUM\n");
      }
      else if(tokens[i]->tag == ID) {
        printf("ID\n");
      }
      else if(tokens[i]->tag == TRUE) {
        printf("TRUE\n");
      }
      else if(tokens[i]->tag == FALSE) {
        printf("FALSE\n");
      }
      else {
        printf("NO MATCHING TAG\n");
      }

      if(tokens[i]->tag == NUM) {
        printf("Token Value: %d\n", tokens[i]->value);
      }
      else if(tokens[i]->tag == ID || tokens[i]->tag == TRUE || tokens[i]->tag == FALSE) {
        printf("Token Lexeme: %s\n", tokens[i]->lexeme);
      }
      else {
        printf("NO MATCHING VALUE / LEXEME");
      }

      printf("----------\n");
    }
  }

  return 0;
}
