#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "hashtable.h"

static unsigned long hash(unsigned char *str)
{
    unsigned long hash = 5381;
    int c;

    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

static int get_slot_index(HashTable *ht, char *key)
{
    return hash((unsigned char *)key) % ht->total_size;
}

static int get_empty_key(HashTableEntry *entry)
{
    for (int i = 0; i < entry->size; i++)
    {
        if (entry->bucket[i].key[0] == '\0')
            return i;
    }

    return -1;
}

static int search_key(HashTableEntry *entry, char *key)
{
    for (int i = 0; i < entry->size; i++)
    {
        if (strcmp(entry->bucket[i].key, key) == 0)
            return i;
    }

    return -1;
}

int ht_delete(HashTable *ht, char *key)
{
    if (strlen(key) == 0)
    {
        printf("Cannot delete empty key\n");
        return 2;
    }

    // Find correct slot
    int slot = get_slot_index(ht, key);

    HashTableEntry *entry = ht->memory + slot;
    int index = search_key(entry, key);
    if (index == -1)
        return 1;

    // No need to delete value
    ht->memory[slot].bucket[index].key[0] = '\0'; // Key empty, marked as deleted
    return 0;
}

int ht_get(HashTable *ht, char *key, int *out_value)
{
    if (strlen(key) == 0)
    {
        printf("Cannot get empty key\n");
        return 2;
    }

    // Find correct slot
    int slot = get_slot_index(ht, key);

    HashTableEntry *entry = ht->memory + slot;
    int index = search_key(entry, key);
    if (index == -1)
    {
        *out_value = 0;
        return 1;
    }

    *out_value = ht->memory[slot].bucket[index].value;
    return 0;
}

int ht_set(HashTable *ht, char *key, int value)
{
    if (strlen(key) == 0)
    {
        printf("Cannot set empty key\n");
        return 2;
    }

    // Find correct slot
    int slot = get_slot_index(ht, key);
    printf("\e[90m(putting key %s at slot %d)\e[0m\n", key, slot);

    HashTableEntry *entry = ht->memory + slot;
    int index = search_key(entry, key);
    if (index == -1) // We need to create it
    {
        // Search if we already have an empty key somewhere
        index = get_empty_key(entry);
        if (index == -1) // No empty space, we need to realloc
        {
            // We'll have space just after current size after realloc
            index = entry->size;

            if (entry->size == 0)
                entry->size = 1;
            else
                entry->size *= 2;

            entry->bucket = realloc(entry->bucket, sizeof(HashTableElement) * entry->size);
            memset(entry->bucket + index, 0, (entry->size - index) * sizeof(HashTableElement)); // Set new elements to 0
        }
    }

    // Set key
    HashTableElement *elem = entry->bucket + index;
    strncpy(elem->key, key, MAX_KEY_SIZE); // Sometime we do that for nothing (overwrite)
    elem->value = value;

    return 0;
}

HashTable *ht_create(int size)
{
    HashTable *ht = malloc(sizeof(HashTable));
    ht->total_size = size;
    ht->memory = calloc(ht->total_size, sizeof(HashTableEntry)); // So it's set to 0

    printf("Allocated %ld bytes\n", ht->total_size * sizeof(HashTableEntry));

    return ht;
}

void ht_dump(HashTable *ht)
{
    printf("==== HASHTABLE DUMP ====\n");
    for (int slot = 0; slot < ht->total_size; slot++)
    {
        printf("SLOT %d\n", slot);
        HashTableEntry *entry = ht->memory + slot;
        if (entry->size == 0)
        {
            printf("    (empty)\n");
        }
        else
        {
            for (int index = 0; index < entry->size; index++)
            {
                HashTableElement *element = entry->bucket + index;
                if (element->key[0] == '\0')
                    printf("    (available)\n");
                else
                    printf("    \"%s\" -> \"%d\"\n", element->key, element->value);
            }
        }
    }
    printf("======= DUMP END =======\n");
}

void ht_destroy(HashTable *ht)
{
    for (int slot = 0; slot < ht->total_size; slot++)
    {
        if (ht->memory[slot].bucket != NULL)
            free(ht->memory[slot].bucket);
    }

    free(ht->memory);
    free(ht);
}
