#include <stdio.h>

char size_buffer[50];

static char *readable_fs(double size_bytes)
{
    int i = 0;
    const char *units[] = {"B", "kB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB"};
    while (size_bytes > 1024)
    {
        size_bytes /= 1024;
        i++;
    }
    snprintf(size_buffer, 49, "%.*f %s", i, size_bytes, units[i]);
    return size_buffer;
}
