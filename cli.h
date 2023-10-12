#include "hashtable.h"

void parse_msg(int out_fd, HashTable *ht, char *msg);
void del(int out_fd, HashTable *ht, char *saveptr);
void get(int out_fd, HashTable *ht, char *saveptr);
void set(int out_fd, HashTable *ht, char *saveptr);
