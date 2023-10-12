#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "hashtable.h"
#include "cli.h"

#include <readline/readline.h>
#include <readline/history.h>

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
            add_history(read);
            parse_msg(STDOUT_FILENO, ht, read);
            free(read_save);
        }
    }
}
