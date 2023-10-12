#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "cli.h"
#include "hashtable.h"

#include <readline/readline.h>
#include <readline/history.h>

void parse_msg(int out_fd, HashTable *ht, char *msg)
{
    char *line;
    char *line_saveptr;
    while ((line = strtok_r(msg, "\n", &line_saveptr)))
    {
        char buffer_line[256];
        strncpy(buffer_line, line, 255);

        char *saveptr;
        char *cmd = strtok_r(line, " ", &saveptr);
        if (strcmp(cmd, "set") == 0)
        {
            set(out_fd, ht, saveptr);
        }
        else if (strcmp(cmd, "get") == 0)
        {
            get(out_fd, ht, saveptr);
        }
        else if (strcmp(cmd, "del") == 0)
        {
            del(out_fd, ht, saveptr);
        }
        else if (strcmp(cmd, "dump") == 0)
        {
            ht_dump(out_fd, ht);
        }
        else
        {
            dprintf(out_fd, "Unknown command\n");
        }

        msg = NULL; // For strok_r
    }
}

void del(int out_fd, HashTable *ht, char *saveptr)
{
    char *arg1 = strtok_r(NULL, " ", &saveptr);
    if (arg1 == NULL)
    {
        dprintf(out_fd, "Syntax: del <key>\n");
        return;
    }

    int deleted = 0;
    do
    {
        int result = ht_delete(ht, arg1);
        if (result == 0)
            deleted++;
    } while ((arg1 = strtok_r(NULL, " ", &saveptr)) != NULL);

    dprintf(out_fd, "%d\n", deleted);
}

void get(int out_fd, HashTable *ht, char *saveptr)
{
    char *arg1 = strtok_r(NULL, " ", &saveptr);
    if (arg1 == NULL)
    {
        dprintf(out_fd, "Syntax: get <key>\n");
        return;
    }

    int value;
    int result = ht_get(ht, arg1, &value);
    if (result == 0)
    {
        dprintf(out_fd, "\"%d\"\n", value);
    }
    else if (result == 1)
    {
        dprintf(out_fd, "(null)\n");
    }
}

void set(int out_fd, HashTable *ht, char *saveptr)
{
    char *arg1 = strtok_r(NULL, " ", &saveptr);
    char *arg2 = strtok_r(NULL, " ", &saveptr);
    if (arg1 == NULL || arg2 == NULL)
    {
        dprintf(out_fd, "Syntax: set <key> <value>\n");
        return;
    }

    int value = atoi(arg2);
    if (value == 0 && strncmp(arg2, "0", 1) != 0)
    {
        dprintf(out_fd, "Unable to parse value '%s'\n", arg2);
        return;
    }

    if (ht_set(ht, arg1, value))
    {
        dprintf(out_fd, "Error when setting key\n");
    }
    else
    {
        dprintf(out_fd, "Ok\n");
    }
}
