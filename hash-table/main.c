#include <stdio.h>
#include <stdlib.h>

#include"lib/hash-table.h"

#define DEBUG 0

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
  char** keys = malloc(sizeof(char*) * 100);
  for (size_t i = 1; i < 100; i++) {
    char* key = malloc(sizeof(char) * 4);
    sprintf(key, "%d", (int)i);

    keys[i] = key;
    values[i] = i;

    if (DEBUG) {
      printf(
        "setting %s to be %d\n",
        keys[i],
        values[i]
      );
    }

    ht_set(
      new_hash_table,
      (const char*)keys[i],
      (void*)&values[i]
    );
    if (DEBUG) printf("\n");
  }

  for (size_t i = 1; i < 100; i++) {
    int* v = (int*)ht_get(
      new_hash_table,
      (const char*)keys[i]
    );

    if (DEBUG) {
      printf(
        "value at %s is %d\n",
        keys[i],
        *v
      );
    }
    
    if (v != &values[i]) {
      if (DEBUG) printf("error at %d", (int)i);
    }
  }

  ht_destroy(new_hash_table);

  free(values);
  for (size_t i = 1; i < 100; i++) {
    free((void*)keys[i]);
  }
  free(keys);
  return 0;
}

