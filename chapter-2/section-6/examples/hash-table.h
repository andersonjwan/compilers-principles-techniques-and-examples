#ifndef HASH_TABLE_H
#define HASH_TABLE_H

/* DEFINE TYPES HERE */
enum datatypes {TOKENS = 501};

/* data structure(s) */
typedef struct {
  char *key;
  void *value;
} ht_item;

typedef struct {
  unsigned int size;  // size of the HashTable
  unsigned int base;  // initial size of HashTable
  unsigned int count; // number of items currently stored

  int type;
  ht_item **items;    // array of HashItems
} ht_table;

/* forward declaration(s) */
ht_table * ht_table_new(const int);
static ht_table * ht_table_new_sized(const int, const int);
static ht_item * ht_item_new(const char *, void *);

void ht_table_del(ht_table *);
static void ht_item_del(ht_item *, const int);

static int ht_hash(const char *, const int, const int);
static int ht_hash_get(const char *, const int, const int);

void ht_insert(ht_table *, const char *, void *);
void * ht_search(ht_table *, const char *);
void ht_delete(ht_table *, const char *);

static void ht_resize(ht_table *, const int);
static void ht_resize_up(ht_table *);
static void ht_resize_down(ht_table *);

#endif
