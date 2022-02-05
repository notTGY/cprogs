#include<stdio.h>
#include"lib/hash-table.h"

int main() {
  ht* my_hash_table = ht_create();

  char* name = "Alice";

  ht_set(my_hash_table, "name", name);

  printf("%s", (char*)ht_get(my_hash_table, "name"));

  ht_destroy(my_hash_table);
}

