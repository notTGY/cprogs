#include "hash-table.h"
#include <stdlib.h>

struct ht {
  char** keys;

  int len;
};

ht* ht_create() {
  ht* table = malloc(sizeof(ht)); 
  table->len = 100;
  table->keys = malloc(sizeof(char*) * table->len);
  return table;
}

void ht_destroy(ht* table) {

}

// void* ht_get(ht* table, const char* key);

// const char* ht_set(
//   ht* table, const char* key, void* value
// );

