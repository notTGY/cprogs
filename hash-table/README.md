# hash table

**Coverage: 97.44%**

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


ht* my_hash_table = ht_create(NULL);
char* name = "Alice";
ht_set(my_hash_table, "name", name, NULL);
printf("%s", (char*)ht_get(my_hash_table, "name"));
// Expected output: "Alice"
ht_destroy(my_hash_table);
```

## Functions

### ht_create

Creates hash table with length of 
`MIN_HASH_TABLE_SIZE`, which can be overwritten in
`hash-table.h`. Initializes all elements.

Pass in `HTERR* err` to get more info about error.
(see `HTERR`)

If there are some errors (allocation),
it will return `NULL`. Otherwise, new hash table.

Usage: `ht* new_table = ht_create(HTERR* err);`

### ht_destroy

Destroys table and all associated data.

Usage: `ht_destroy();`

### ht_set

Sets value of the key in the table. Automatically
resizes table. Resize size can be controlled with
`HASH_TABLE_SIZE_INC` in `hash-table.h`

Important! key passed shouldn't be `NULL`

On error returns `NULL`
If key-value was inserted successfully - returns key
(duplicated string if there wasn't such key in the
table)

Pass in `HTERR* err` to get more info about error.
(see `HTERR`)

Usage: `ht_set(new_table, "Alice", (void*)&x, HTERR* err);`

### ht_get

Returns value of the element with a given key.
If there is no such key, returns `NULL`.

Usage: `int x = *(int*)ht_get(new_table, "Alice");`

## Enums

### HTERR

Enum has following values:

```
ESUCCESS
EINVARG
EMALLOC
```

