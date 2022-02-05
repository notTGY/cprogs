# hash table

## Introduction

This is a hash table lib done as a school project
at my education, so it doesn't pretend to be
interesting or innovative.

Basically, hash table is a data structure, that lets
you store key-value pairs effectively. Underhood it
uses hash algorithm to translate key into the
corresponding hash and store an element somewhere.

## Simple example of usage

```
#include <stdio.h>
#include "lib/hash-table.c"
...


ht* my_hash_table = ht_create();
char* name = "Alice";
ht_set(my_hash_table, "name", name);
printf(ht_get(my_hash_table, "name");
// Expected output: "Alice"
ht_destroy(my_hash_table);
```

## Functions

### ht_create

### ht_destroy

### ht_set

### ht_get

