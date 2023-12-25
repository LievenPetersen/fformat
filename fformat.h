/* 
 * Header-only-library for simple definition of custom file formats.
 *
 * See example.c on how it is intended to be used.
 *
 * Config:
 * #define FFORMAT_IMPL // to access function definitions
 *
 * Most functions return a bool that is true on success
 * or false if the data could not be written to/read from the file.
 *
 * Pitfalls:
 * - be aware, that any change to a file format will prevent users from
 *   successfully loading files that were saved with the old format.
 *   Consider using version identifiers within the file format and supporting
 *   the loading of legacy files.
 */


#ifndef __FFORMAT_H
#define __FFORMAT_H

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

typedef enum{
    ACTION_LOAD,
    ACTION_SAVE,
}ACTION_ENUM;

// open a file in the mode indicated by direction (needs to be closed manually)
static FILE *with_file(char *path, ACTION_ENUM direction);
// define the next section to contain the given string. str needs to be NULL terminated.
// NULL terminator will not be written to file.
static bool str_literal(FILE *file, ACTION_ENUM direction, char *str);
// pointer to a memory location of n_bytes size. Data is written/read as raw bytes.
static bool var_content(FILE *file, ACTION_ENUM direction, void *var, size_t n_bytes);

#endif // __FFORMAT_H

// unlock definitons
#ifdef FFORMAT_IMPL

// open a file in the mode indicated by direction (needs to be closed manually)
static FILE *with_file(char *path, ACTION_ENUM direction){
    switch(direction){
        case ACTION_LOAD: return fopen(path, "rb");
        case ACTION_SAVE: return fopen(path, "wb");
    }
    return NULL;
}

// define the next section to contain the given string. str needs to be NULL terminated.
// NULL terminator will not be written to file.
static bool str_literal(FILE *file, ACTION_ENUM direction, char *str){
    size_t str_len = strlen(str);
    switch (direction) {
        case ACTION_LOAD: {
            char buf[str_len];
            return fread(buf, 1, str_len, file) == str_len 
                && strncmp(str, buf, str_len) == 0;
        }break;
        case ACTION_SAVE: {
            return fwrite(str, 1, str_len, file) == str_len;
        }break;
    }
    return false;
}

// pointer to a memory location of n_bytes size. Data is written/read as raw bytes.
static bool var_content(FILE *file, ACTION_ENUM direction, void *var, size_t n_bytes){
    switch (direction) {
        case ACTION_LOAD: {
            return fread(var, 1, n_bytes, file) == n_bytes;
        }break;
        case ACTION_SAVE: {
            return fwrite(var, 1, n_bytes, file) == n_bytes;
        }break;
    }
    return false;
}

#endif // FFORMAT_IMPL
