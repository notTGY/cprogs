#ifndef _HASH_TABLE_TGY
#define _HASH_TABLE_TGY

/**
 * Hash table structure:
 * create with ht_create;
 * free with ht_destroy;
 */
typedef struct ht ht;

/**
 * Create hash table and return pointer to it;
 * in case something goes wrong -> NULL
 */
ht* ht_create();

/**
 * Destroy hash table and all associated data
 */
void ht_destroy(ht* table);

/**
 * Get item with a given key
 */
void* ht_get(ht* table, const char* key);

/**
 * Set item with a given key
 * If key is not present in the table,
 * it is copied to newly allocated memory
 */
const char* ht_set(
  ht* table, const char* key, void* value
);

#endif
