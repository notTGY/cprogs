#include <stdio.h>
#include <stdlib.h>

#include"lib/hash-table.h"

#define DEBUG 1

int main() {
  ht* my_hash_table = ht_create();

  char* name = "Alice";

  ht_set(my_hash_table, "name", name);

  if (DEBUG) {
    printf("1. expected \"Alice\", got: \"%s\"\n", (char*)ht_get(my_hash_table, "name"));
  }

  ht_destroy(my_hash_table);


  ht* new_hash_table = ht_create();

  int* values = malloc(sizeof(int) * 100);
  for (size_t i = 1; i < 100; i++) {
    char* key = "key_x";
    key[4] = i;
    values[i] = i;

    ht_set(
      new_hash_table,
      (const char*)key,
      (void*)&values[i]
    );
  }

  for (size_t i = 1; i < 100; i++) {
    char* key = "key_x";
    key[4] = i;

    int* v = (int*)ht_get(
      new_hash_table,
      (const char*)key
    );
    
    if (v != &values[i]) {
      if (DEBUG) printf("error at %d", (int)i);
    }
  }

}

