#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "hashtable.h"

#include <readline/readline.h>
#include <readline/history.h>

void del(HashTable *ht, char *saveptr)
{
    char *arg1 = strtok_r(NULL, " ", &saveptr);
    if (arg1 == NULL)
    {
        printf("Syntax: del <key>\n");
        return;
    }

    int deleted = 0;
    do
    {
        int result = ht_delete(ht, arg1);
        if (result == 0)
            deleted++;
    } while ((arg1 = strtok_r(NULL, " ", &saveptr)) != NULL);

    printf("%d\n", deleted);
}

void get(HashTable *ht, char *saveptr)
{
    char *arg1 = strtok_r(NULL, " ", &saveptr);
    if (arg1 == NULL)
    {
        printf("Syntax: get <key>\n");
        return;
    }

    int value;
    int result = ht_get(ht, arg1, &value);
    if (result == 0)
    {
        printf("\"%d\"\n", value);
    }
    else if (result == 1)
    {
        printf("(null)\n");
    }
}

void set(HashTable *ht, char *saveptr)
{
    char *arg1 = strtok_r(NULL, " ", &saveptr);
    char *arg2 = strtok_r(NULL, " ", &saveptr);
    if (arg1 == NULL || arg2 == NULL)
    {
        printf("Syntax: set <key> <value>\n");
        return;
    }

    int value = atoi(arg2);
    if (value == 0 && strncmp(arg2, "0", 1) != 0)
    {
        printf("Unable to parse value '%s'\n", arg2);
        return;
    }

    if (ht_set(ht, arg1, value))
    {
        printf("Error when setting key\n");
    }
    else
    {
        printf("Ok\n");
    }
}

int main(int argc, char const *argv[])
{
    HashTable *ht = ht_create(0xF);

    using_history();

    while (1)
    {
        char *read = readline("bedis> ");
        if (read == NULL)
        {
            rl_clear_history();
            ht_destroy(ht);
            return 0;
        }
        else
        {
            char *read_save = read;
            char *line;
            char *line_saveptr;
            while ((line = strtok_r(read, "\n", &line_saveptr)))
            {
                char buffer_line[256];
                strncpy(buffer_line, line, 255);

                add_history(line);
                char *saveptr;
                char *cmd = strtok_r(line, " ", &saveptr);
                if (strcmp(cmd, "set") == 0)
                {
                    set(ht, saveptr);
                }
                else if (strcmp(cmd, "get") == 0)
                {
                    get(ht, saveptr);
                }
                else if (strcmp(cmd, "del") == 0)
                {
                    del(ht, saveptr);
                }
                else if (strcmp(cmd, "dump") == 0)
                {
                    ht_dump(ht);
                }
                else
                {
                    printf("Unknown command\n");
                }

                read = NULL; // For strok_r
            }

            free(read_save);
        }
    }
}
