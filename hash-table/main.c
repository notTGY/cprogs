#include<stdio.h>
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
}

