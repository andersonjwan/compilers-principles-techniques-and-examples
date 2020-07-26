#ifndef HASH_H
#define HASH_H

/* standard libarary header(s) */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* RESTRICTION(S) */
#define INIT_SIZE 100

/* data structure definition(s) */
typedef struct {
  char *key;   // item key
  void *value; // item value
} hash_item;

typedef struct {
  unsigned int size;  // max table size
  unsigned int base;  // the base table size (does not change)
  unsigned int count; // number of items in table currently

  void (*garbage) (void *value);
  hash_item **items;  // array of pointers to item(s)
} hash_table;

/* global(s) */
static hash_item HASH_ITEM_DEL = {NULL, NULL};

/* forward declaration(s) */
hash_table * new_hash_table(void (*garbage) (void *));
static hash_table * new_hash_table_sized(const int, void (*garbage) (void *));
void del_hash_table(hash_table *);
static void del_hash_table_item_key(hash_table *);

static hash_item * new_hash_item(const char *, void *);
static void del_hash_item(hash_table *, hash_item *);
static void del_hash_item_key(hash_item *);

static int hash_index(const char *, const int, const int);
static int get_hash_index(const char *, const int, int);

void hash_insert(hash_table *, const char *, void *);
void * hash_search(hash_table *, const char *);
void hash_delete(hash_table *, const char *);
void hash_print(hash_table *);

static void hash_resize(hash_table *, const int);
static void hash_resizeup(hash_table *);
static void hash_resizedown(hash_table *);

static void print_event(const char *);
static void check_alloc(const void *);

static int is_prime(int);
static int next_prime(int);
static unsigned long power(int, int);

/* function definition(s) */
hash_table * new_hash_table(void (*garbage) (void *value)) {
  hash_table *new_table;
  new_table = new_hash_table_sized(INIT_SIZE, garbage);

  return new_table;
}

static hash_table * new_hash_table_sized(const int base,
                                         void (*garbage) (void *value)) {
  hash_table *new_table;
  new_table = (hash_table *) malloc(sizeof(hash_table));
  check_alloc((void *) new_table);

  new_table->base = base;
  new_table->size = next_prime(new_table->base);
  new_table->count = 0;
  new_table->garbage = garbage;

  new_table->items = calloc((size_t) new_table->size, sizeof(hash_item *));
  check_alloc((void *) new_table->items);

  return new_table;
}

void del_hash_table(hash_table *table) {
  for(int i = 0; i < table->size; ++i) {
    hash_item *item;
    item = table->items[i];

    if(item != NULL && item != &HASH_ITEM_DEL) {
      del_hash_item(table, item);
    }
  }

  free(table->items);
  free(table);
}

static void del_hash_table_item_key(hash_table *table) {
  for(int i = 0; i < table->size; ++i) {
    hash_item *item;
    item = table->items[i];

    if(item != NULL & item != &HASH_ITEM_DEL) {
      del_hash_item_key(item);
    }
  }

  free(table->items);
  free(table);
}

static hash_item * new_hash_item(const char *key, void *value) {
  hash_item *new_item;
  new_item = (hash_item *) malloc(sizeof(hash_item));

  new_item->key   = strdup(key);
  new_item->value = value;

  return new_item;
}

static void del_hash_item(hash_table *table, hash_item *item) {
  void *value;
  value = item->value;

  free(item->key);
  free(item);

  (*(table->garbage)) (value);
}

static void del_hash_item_key(hash_item *item) {
  free(item->key);
  free(item);
}

static int hash_index(const char *key, const int prime, const int size) {
  unsigned long hash = 0;
  const int length = strlen(key);

  for(int i = 0; i < length; ++i) {
    hash += (unsigned long) power(prime, length - (i + 1)) * key[i];
    hash = hash % size;
  }

  return (int) hash;
}

static int get_hash_index(const char *key, const int size, const int attempt) {
  const int hash_01 = hash_index(key, 5381, size);
  const int hash_02 = hash_index(key, 7129, size);

  return (hash_01 + (attempt * (hash_02 + 1))) % size;
}

void hash_insert(hash_table *table, const char *key, void *value) {
  const int load = (table->count * 100) / table->size;
  if(load > 70) {
    hash_resizeup(table);
  }

  hash_item *item;
  item = new_hash_item(key, value);

  int index;
  index = get_hash_index(item->key, table->size, 0);

  hash_item *curr;
  curr = table->items[index];

  int i = 1;
  while(curr != NULL) {
    if(curr != &HASH_ITEM_DEL) {
      if(strcmp(curr->key, key) == 0) {
        del_hash_item(table, curr);

        /* update table */
        table->items[index] = item;
        return;
      }
    }

    index = get_hash_index(item->key, table->size, i);
    print_event("Collision Detected");

    curr = table->items[index];
    ++i;
  }

  table->items[index] = item;
  ++(table->count);
}

void * hash_search(hash_table *table, const char *key) {
  int index;
  index = get_hash_index(key, table->size, 0);

  hash_item *item;
  item = table->items[index];

  int i = 1;
  while(item != NULL) {
    if(item != &HASH_ITEM_DEL) {
      if(strcmp(item->key, key) == 0) {
        return item->value;
      }
    }

    index = get_hash_index(key, table->size, i);
    item = table->items[index];
    ++i;
  }

  return NULL;
}

void hash_delete(hash_table *table, const char *key) {
  const int load = (table->count * 100) / table->size;
  if(load < 10) {
    hash_resizedown(table);
  }

  int index;
  index = get_hash_index(key, table->size, 0);

  hash_item *item;
  item = table->items[index];

  int i = 1;
  while(item != NULL) {
    if(item != &HASH_ITEM_DEL) {
      if(strcmp(item->key, key) == 0) {
        del_hash_item(table, item);           // delete the old item
        table->items[index] = &HASH_ITEM_DEL; // set item as DELETED
      }
    }

    index = get_hash_index(key, table->size, i);
    item = table->items[index];
    ++i;
  }

  --(table->count);
}

void hash_print(hash_table *table) {
  hash_item *item;
  int active = 1;

  for(int i = 0; i < table->size; ++i) {
    item = table->items[i];

    if(item != NULL) {
      printf("Item #%02d @ Index %d\n", active, i);
      printf("Data: {%s, %p}\n", item->key, (char *) item->value);
      ++active;
    }
  }
}

static void hash_resize(hash_table *table, const int base) {
  if(base < INIT_SIZE) {
    return;
  }

  hash_table *new_table;
  new_table = new_hash_table_sized(base, table->garbage);

  for(int i = 0; i < table->size; ++i) {
    hash_item *item;
    item = table->items[i];

    if(item != NULL && item != &HASH_ITEM_DEL) {
      hash_insert(new_table, item->key, item->value);
    }
  }

  table->base = new_table->base;
  table->count = new_table->count;

  const int tmp_size = table->size;
  table->size = new_table->size;
  new_table->size = tmp_size;

  hash_item **tmp_items = table->items;
  table->items = new_table->items;
  new_table->items = tmp_items;

  del_hash_table_item_key(new_table); // delete the "old" table
}

static void hash_resizeup(hash_table *table) {
  const int new_base = (table->base) * 2;
  hash_resize(table, new_base);
}

static void hash_resizedown(hash_table *table) {
  const int new_base = (table->base) / 2;

  hash_resize(table, new_base);
}

static void print_event(const char *event) {
  fprintf(stdout, "\033[0;33m");    // set the text color to bold yellow
  fprintf(stdout, "EVENT");
  fprintf(stdout, "\033[0m");       // reset the text color to default

  fprintf(stdout, ": %s\n", event); // print message
}

static void check_alloc(const void *ptr) {
  if(ptr == NULL) {
    fprintf(stderr, "Error : Insufficient Memory\n");
    exit(1);
  }

  return;
}

/* HELPER FUNCTION(S) */
static int is_prime(int num) {
  if(num <= 1) { return 0; }
  if(num <= 3) { return 1; }

  if(num % 2 == 0 || num % 3 == 0) {
    return 0;
  }

  for(int i = 5; i * i <= num; i = i + 6) {
    if(num % i == 0 || (num % (i + 2)) == 0) {
      return 0;
    }
  }

  return 1;
}

static int next_prime(int num) {
  if(num <= 1) {
    return 2;
  }

  int prime = num;
  int found = 0;

  while(!found) {
    ++prime;

    if(is_prime(prime)) {
      found = 1;
    }
  }

  return prime;
}

static unsigned long power(int a, int b) {
  if(b == 0) {
    return 1;
  }

  unsigned long result = a;
  for(int i = 1; i < b; ++i) {
    result = result * a;
  }

  return result;
}

#endif
