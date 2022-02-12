#include "hash-table.h"
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

typedef struct {
  const char* key;
  void* value;
} elem;

struct ht {
  elem* elems;

  size_t len;
  size_t used;
};

ht* ht_create(HTERR* err) {
  ht* table = malloc(sizeof(ht));
  if (err) *err = EMALLOC;
  if (!table) return NULL;

  table->len = MIN_HASH_TABLE_SIZE;
  table->used = 0;

  table->elems = malloc(sizeof(elem) * table->len);
  if (err) *err = EMALLOC;
  if (!table->elems) {
    free(table);
    return NULL;
  }

  for (size_t i = 0; i < table->len; i++) {
    table->elems[i].key = NULL;
  }

  if (err) *err = ESUCCESS;
  return table;
}

void ht_destroy(ht* table) {
  for (size_t i = 0; i < table->len; i++) {
    const void* allocated_key = table->elems[i].key;
    if (allocated_key) free((void*)allocated_key);
  }

  free(table->elems);
  free(table);
}


// https://en.wikipedia.org/wiki/Fowler–Noll–Vo_hash_function
#define OFFSET 14695981039346656037UL
#define PRIME 1099511628211UL
uint64_t hash(const char* key) {
  uint64_t hash = OFFSET;
  for (const char* p = key; *p; p++) {
    hash ^= (uint64_t)(unsigned char)(*p);
    hash *= PRIME;
  }

  return hash;
}

/**
 * Get index in table by key
 * 1. get hash
 * 2. get corresponding index
 */
size_t get_idx_for_key(size_t len, const char* key) {
  uint64_t _hash = hash(key);
  size_t i = (size_t)(_hash % (uint64_t)len);
  return i;
}

void* ht_get(ht* table, const char* key) {
  size_t i = get_idx_for_key(table->len, key);

  while (table->elems[i].key) {
    if (strcmp(key, table->elems[i].key) == 0)
      return table->elems[i].value;

    if (++i >= table->len) i = 0;
  }

  return NULL;
}

/**
 * Function to set elem, when we are sure that we
 * have space for it
 *
 * We give so much parameters to reuse this func
 * for expanding the table
 *
 * on success return key, on error return NULL
 */
const char* ht_set_elem(
  elem* elems,
  size_t len,
  const char* key,
  void* value,
  size_t* used
) {
  size_t i = get_idx_for_key(len, key);
  while (elems[i].key) {
    if (strcmp(key, elems[i].key) == 0) {
      elems[i].value = value;
      return elems[i].key;
    }

    if (++i >= len) i = 0;
  }

  // key not in table
  if (used) {
    key = strdup(key);
    if (key == NULL) return NULL;
    (*used)++;
  }
  elems[i].key = key;
  elems[i].value = value;

  return elems[i].key;
}

/**
 * resize hash table and keep all previous items
 * on success return 0, on error 1
 */
int ht_resize(ht* table) {
  size_t new_len = table->len + HASH_TABLE_SIZE_INC;
  // type overflow
  if (new_len < table->len) return 1;
  
  elem* new_elems = malloc(sizeof(elem) * new_len);
  if (!new_elems) return 1;

  for (size_t i = 0; i < new_len; i++) {
    new_elems[i].key = NULL;
  }

  for (size_t i = 0; i < table->len; i++) {
    elem el = table->elems[i];

    if (el.key)
      ht_set_elem(
        new_elems,
        new_len,
        el.key,
        el.value,
        NULL
      );
  }

  free(table->elems);
  table->elems = new_elems;
  table->len = new_len;
  return 0;
}

/**
 * set item in hash table
 * on success return key, on error return NULL
 */
const char* ht_set(
  ht* table, const char* key, void* value
) {

  if (!key) return NULL;

  // check if we need resize
  if (
    table->used > table->len - HASH_TABLE_SIZE_INC - 1
  ) {
    if (ht_resize(table)) return NULL;
  }

  return ht_set_elem(
    table->elems,
    table->len,
    key,
    value,
    &table->used
  );
}

