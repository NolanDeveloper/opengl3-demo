#include <stdio.h>
#include <stdlib.h>

extern void * 
emalloc(size_t size) {
    void * p = malloc(size);
    if (!p) exit(1);
    return p;
}

#define KILOBYTE 1024
#define MEGABYTE (1024 * KILOBYTE)

extern char *
loadFile(const char * filepath) {
    FILE * f = fopen(filepath, "rb");
    if (!f) exit(1);
    if (-1 == fseek(f, 0, SEEK_END)) exit(1);
    long fileSize = ftell(f);
    if (4 * MEGABYTE < fileSize) exit(1);
    if (-1 == fileSize) exit(1);
    if (-1 == fseek(f, 0, SEEK_SET)) exit(1);
    char * string = emalloc(fileSize + 1);
    if ((size_t)fileSize > fread(string, 1, fileSize, f)) exit(1);
    fclose(f);
    string[fileSize] = 0;
    return string;
}
