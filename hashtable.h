#pragma once

#define MAX_KEY_SIZE 50

typedef struct s_HashTableElement
{
    char key[MAX_KEY_SIZE + 1];
    int value;
} HashTableElement;

typedef struct s_HashTableEntry
{
    int size;
    HashTableElement *bucket;
} HashTableEntry;

typedef struct s_HashTable
{
    int total_size;
    HashTableEntry *memory;
} HashTable;

void ht_dump(int out_fd, HashTable *ht);
int ht_delete(HashTable *ht, char *key);
int ht_set(HashTable *ht, char *key, int value);
int ht_get(HashTable *ht, char *key, int *out_value);
HashTable *ht_create(int size);
void ht_destroy(HashTable *ht);
