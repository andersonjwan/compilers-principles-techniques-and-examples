#include <stdio.h>
#include <stdlib.h>

#include "hash.h"

/* data structure(s) */
typedef struct {
  Token t;
} Symbol;

typedef struct {
  hash_table *sym_table;
  struct Environments *prev;
} Environment;

/* global(s) */
static Environment *top;

void symbol_deconstructor(void *value) {
  Symbol *sym;
  sym = (Symbol *) value;

  free(sym);
}

void env_new(Environment *prev) {
  Environment *env;
  env = (Environment *) malloc(sizeof(Environment));

  if(env == NULL) {
    fprintf(stderr, "Memory Allocation Error : Unable to create Environemnt\n");
    exit(1);
  }

  env->prev = prev;
  env->table = new_hash_table(symbol_deconstructor);
  env->next = 0;
}

void env_put(Environment *env, char *s, Symbol *sym) {
  hash_table *table;
  table = env->sym_table;

  hash_insert(table, s, sym);
}

Symbol * env_get(Environment *env, char *s) {
  Environment *curr;

  /* work bottom-up */
  for(curr = env; curr != NULL; curr = curr->prev) {
    Symbol *found;
    found = hash_search(curr->sym_table, s);

    if(found != NULL) {
      return found;
    }
  }

  return NULL;
}
