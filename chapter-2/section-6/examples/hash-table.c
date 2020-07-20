#include <stdlib.h>
#include <stdio.h>

#include <math.h>
#include <string.h>

#include "hash-table.h"
#include "hash-prime.h"

#include "ex-2.6.h"

/* macro(s) */
#define HT_INIT_BASE_SIZE 10

#define HT_PRIME_01 next_prime(128)
#define HT_PRIME_02 next_prime(HT_PRIME_01)

/* global(s) */
static ht_item HT_DELETED_ITEM = {NULL, NULL};

static ht_table * ht_table_new_sized(const int size, const int type) {
  ht_table *table = (ht_table *) malloc(sizeof(ht_table));
  if(table == NULL) {
    fprintf(stderr, "Memory Alloc. Error : Unable to create new table.\n");
    exit(1);
  }

  table->base = size;
  table->size = next_prime(table->base);

  table->count = 0;

  table->type = type;
  table->items = (ht_item **) calloc((size_t) table->size, sizeof(ht_item *));
  if(table->items == NULL) {
    fprintf(stderr, "Memory Calloc. Error : Unable to set table size.\n");
    exit(2);
  }

  return table;
}

ht_table * ht_table_new(const int type) {
  return ht_table_new_sized(HT_INIT_BASE_SIZE, type);
}

static ht_item * ht_item_new(const char *key, void *value) {
  ht_item *item = (ht_item *) malloc(sizeof(ht_item));
  if(item == NULL) {
    fprintf(stderr, "Memory Alloc. Error : Unable to create new item.\n");
    exit(1);
  }

  /* duplicate key value */
  item->key = strdup(key);
  if(item->key == NULL) {
    fprintf(stderr, "Memory Alloc. Error : Unable to duplicate key value\n");
    exit(1);
  }

  /* set value value */
  item->value = value;

  return item;
}

void ht_table_del(ht_table *table) {
  int size;
  size = table->size;

  for(int i = 0; i < size; ++i) {
    ht_item *item = (table->items)[i];

    if(item != NULL) {
      ht_item_del(item, table->type);
    }
  }

  free(table->items);
  free(table);
}

static void ht_item_del(ht_item *item, const int type) {
  free(item->key);

  free(item);
}

static int ht_hash(const char *s, const int prime, const int table_size) {
  long hash = 0;
  const int len_s = strlen(s);

  for(int i = 0; i < len_s; ++i) {
    hash += (long) pow(prime, len_s - (i + 1)) * s[i];
    hash = hash % table_size;
  }

  return (int) hash;
}

static int ht_hash_get(const char *s, const int table_size, const int attempt) {
  const int hash_01 = ht_hash(s, HT_PRIME_01, table_size);
  const int hash_02 = ht_hash(s, HT_PRIME_02, table_size);

  return (hash_01 + (attempt * (hash_02 + 1))) % table_size;
}

void ht_insert(ht_table *table, const char *key, void *value) {
  const int load = (table->count * 100) / table->size;
  if(load > 70) {
    ht_resize_up(table);
  }

  ht_item *new_item = ht_item_new(key, value);
  int index = ht_hash_get(new_item->key, table->size, 0); // initial attempt = 0

  ht_item *curr_item = table->items[index];
  int i = 1;

  while(curr_item != NULL) {
    if(curr_item != &HT_DELETED_ITEM) {
      if(strcmp(curr_item->key, key) == 0) {
        /* update item's value */
        ht_item_del(curr_item, table->type);
        table->items[index] = new_item;

        return;
      }
    }

    /* next item */
    index = ht_hash_get(new_item->key, table->size, i);

    curr_item = table->items[index];
    ++i;
  }

  /* insert new item to table */
  table->items[index] = new_item;
  ++(table->count);
}

void * ht_search(ht_table *table, const char *key) {
  int index = ht_hash_get(key, table->size, 0);
  ht_item *item = table->items[index];

  int i = 1;
  while(item != NULL) {
    if(item != &HT_DELETED_ITEM) {
      if(strcmp(item->key, key) == 0) {
        return (void *) item->value;
      }
    }

    index = ht_hash_get(key, table->size, i);

    item = table->items[index];
    ++i;
  }

  return NULL;
}

void ht_delete(ht_table *table, const char *key) {
  const int load = (table->count * 100) / table->size;
  if(load < 10) {
    ht_resize_down(table);
  }

  int index = ht_hash_get(key, table->size, 0);
  ht_item *item = table->items[index];

  int i = 1;
  while(item != NULL) {
    if(item != &HT_DELETED_ITEM) {
      if(strcmp(item->key, key) == 0) {
        ht_item_del(item, table->type);
        table->items[index] = &HT_DELETED_ITEM;
      }
    }

    index = ht_hash_get(key, table->size, i);
    item = table->items[index];
    ++i;
  }

  --(table->count);
}

/* RESIZING FUNCTION(S) */

static void ht_resize(ht_table *table, const int base) {
  if(base < HT_INIT_BASE_SIZE) {
    return; // minimum table size
  }

  // new table at size = base
  ht_table *new_table = ht_table_new_sized(base, table->type);
  const int size = table->size; // previous table size

  for(int i = 0; i < size; ++i) {
    /* move non-NULL item(s) to new table */
    ht_item *item = table->items[i];

    if(item != NULL && item != &HT_DELETED_ITEM) {
      ht_insert(new_table, item->key, item->value);
    }
  }

  table->base = new_table->base;
  table->count = new_table->count;

  /* delete new table */
  table->size = new_table->size;
  new_table->size = size;

  ht_item **tmp_items = table->items;
  table->items = new_table->items; // new recently transferred item(s)
  new_table->items = tmp_items;    // old item(s)

  ht_table_del(new_table);
}

static void ht_resize_up(ht_table *table) {
  const int new_size = table->base * 2;
  ht_resize(table, new_size);
}

static void ht_resize_down(ht_table *table) {
  const int new_size = table->base / 2;
  ht_resize(table, new_size);
}
