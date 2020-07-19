#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* terminal(s) */
enum terminals {
                NUM = 256, ID = 257, TRUE = 258, FALSE = 259,
                LESS_THAN = 260, LESS_THAN_EQUAL = 261, EQUAL = 262,
                NOT_EQUAL = 263, GREATER_THAN_EQUAL = 264, GREATER_THAN = 265
};

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
    else if(peek == '/') {
      /* COMMENT(S) */
      char prev = peek;
      peek = (char) getchar();

      if(prev == '/' && peek == '/') {
        /* single-lined comment */
        for( ; ; peek = (char) getchar()) {
          if(peek == '\n') {
            line = line + 1;
            break;
          }
        }
      }
      else if(prev == '/' && peek == '*') {
        /* block-lined comment */
        for( ; ; peek = (char) getchar()) {
          if(peek == '*') {
            peek = (char) getchar();

            if(peek == '/') {
              break;
            }
            else if(peek == '\n') {
              line = line + 1;
            }
          }
          else if(peek == '\n') {
            line = line + 1;
          }
        }
      }
      else {
        /* create and return previous character as token */
        Token *t = (Token *) malloc(sizeof(Token));
        t->tag = prev;
        t->line = line;
        t->value = 0;
        t->lexeme = NULL;

        return *t;
      }
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

    /* create and return new token */
    Token *t = (Token *) malloc(sizeof(Token));
    t->tag = NUM;
    t->line = line;
    t->value = v;
    t->lexeme = NULL;

    return *t;                // RETURN TOKEN
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
    return *t;                // RETURN TOKEN
  }

  /* RELATIONAL OPERATOR(S) */
  if((peek >= 60 && peek <= 62) || (peek == 33)) {
    char prev = peek;
    peek = (char) getchar();

    Token *t = (Token *) malloc(sizeof(Token));

    if(prev == '<') {
      if(peek == '=') {
        /* less than or equal */
        t->tag = LESS_THAN_EQUAL;
        t->line = line;
        t->value = 0;
        t->lexeme = NULL;

        peek = ' ';
        return *t;
      }
      else {
        /* less than */
        t->tag = LESS_THAN;
        t->line = line;
        t->value = 0;
        t->lexeme = NULL;

        peek = ' ';
        return *t;
      }
    }
    else if(prev == '>') {
      if(peek == '=') {
        /* greater than or equal */
        t->tag = GREATER_THAN_EQUAL;
        t->line = line;
        t->value = 0;
        t->lexeme = NULL;

        peek = ' ';
        return *t;
      }
      else {
        /* greater than */
        t->tag = GREATER_THAN;
        t->line = line;
        t->value = 0;
        t->lexeme = NULL;

        peek = ' ';
        return *t;
      }
    }
    else if(prev == '=') {
      if(peek == '=') {
        /* equal */
        t->tag = EQUAL;
        t->line = line;
        t->value = 0;
        t->lexeme = NULL;

        peek = ' ';
        return *t;
      }
    }
    else if(prev == '!') {
      if(peek == '=') {
        /* not equal */
        t->tag = NOT_EQUAL;
        t->line = line;
        t->value = 0;
        t->lexeme = NULL;

        peek = ' ';
        return *t;
      }
    }
  }

  /* create and return current character as Token */
  Token *t = (Token *) malloc(sizeof(Token));

  t->tag = peek;
  t->line = line;
  t->value = 0;
  t->lexeme = NULL;

  peek = ' '; // reset peek to blank-space

  return *t;                  // RETURN TOKEN
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

  Token t;
  while(t.tag != EOF) {
    t = scan();
    printf("Token Tag: %d\n", t.tag);
    printf("Token Line Number: %d\n", t.line);
    printf("Token Value: %d\n", t.value);
    printf("Token Lexeme: %s\n", t.lexeme);
    printf("----------\n");
  }

  return 0;
}
