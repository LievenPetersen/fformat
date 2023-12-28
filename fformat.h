/*  MIT License

    Copyright (c) 2023 Lieven Petersen

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE. */


/* Header-only-library for simple definition, writing and loading of custom file formats.
 *
 * See example.c on how it is intended to be used.
 *
 * CONFIG:
 *   #define FFORMAT_IMPL  // to access function definitions
 *
 *   Allocators can be overwritten by defining the following before the #include fformat.h:
 *   #define FF_MALLOC <your malloc>
 *   #define FF_FREE <your free>
 *   - if one is defined, both need to be defined
 *   - functions that allocate have "alloc" in their name
 *
 * Most functions return a bool that is true on success
 * or false if the data could not be written to/read from the file.
 *
 * Pitfalls:
 * - consider if any standard file formats may already suit your needs.
 * - use stdint.h if you want portability between systems.
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
    FF_MODE_LOAD,
    FF_MODE_SAVE,
}FF_MODE;

// open a file in the indicated mode (needs to be closed manually)
static FILE *ff_open(char *path, FF_MODE mode);
// define the next section to contain the given string. str needs to be NULL terminated.
// NULL terminator will not be written to file.
static bool ff_lit_string(FILE *file, FF_MODE mode, char *str);

// pointer to a memory location of n_bytes size. Data is written/read as raw bytes.
static bool ff_var_bytes(FILE *file, FF_MODE mode, void *var, size_t n_bytes);
// similar to ff_var_bytes, except allocates n_bytes when in MODE_LOAD, thus requiring extra level of '&'.
static bool ff_var_bytes_alloc(FILE *file, FF_MODE mode, void **var_ptr, size_t n_bytes);

// >string< meaning that var expects and yields a NULL terminated string.
// Requires var to point to str_len+1 allocated bytes.
// str_len is still required, because the in-file representation does not have any termination.
// If the string is not NULL terminated use bytes instead.
static bool ff_var_string(FILE *file, FF_MODE mode, char *var, size_t str_len);
// like ff_var_string but allocates str_len+1 bytes.
static bool ff_var_string_alloc(FILE *file, FF_MODE mode, char **var_ptr, size_t str_len);

// a string that ends with given terminator. Will end early if string contains terminator.
static bool ff_var_string_until(FILE *file, FF_MODE mode, char *var, char terminator);
// like ff_var_string_until but allocates str_len+1 bytes.
// a string that ends with given terminator. Will end early if string contains terminator.
static bool ff_var_string_until_alloc(FILE *file, FF_MODE mode, char **var_ptr, char terminator);

#endif // __FFORMAT_H

// include definitons
#ifdef FFORMAT_IMPL

// override malloc and free
#ifdef FF_MALLOC
#   ifndef FF_FREE
#   error "FF_MALLOC was defined by user, but FF_FREE is not!"
#   endif // FF_FREE
#else
#   ifdef FF_FREE
#   error "FF_FREE was defined by user, but FF_MALLOC is not!"
#   endif // FF_FREE
    // provide standard malloc and free
#   include <stdlib.h>
#   define FF_MALLOC malloc
#   define FF_FREE free
#endif // FF_MALLOC

// open a file in the indicated mode (needs to be closed manually)
static FILE *ff_open(char *path, FF_MODE mode){
    switch(mode){
        case FF_MODE_LOAD: return fopen(path, "rb");
        case FF_MODE_SAVE: return fopen(path, "wb");
    }
    return NULL;
}

// define the next section to contain the given string. str needs to be NULL terminated.
// NULL terminator will not be written to file.
static bool ff_lit_string(FILE *file, FF_MODE mode, char *str){
    size_t str_len = strlen(str);
    switch (mode) {
        case FF_MODE_LOAD: {
            char buf[str_len];
            return fread(buf, 1, str_len, file) == str_len 
                && strncmp(str, buf, str_len) == 0;
        }break;
        case FF_MODE_SAVE: {
            return fwrite(str, 1, str_len, file) == str_len;
        }break;
    }
    return false;
}

// pointer to a memory location of n_bytes size. Data is written/read as raw bytes.
static bool ff_var_bytes(FILE *file, FF_MODE mode, void *var, size_t n_bytes){
    switch (mode) {
        case FF_MODE_LOAD: {
            return fread(var, 1, n_bytes, file) == n_bytes;
        }break;
        case FF_MODE_SAVE: {
            return fwrite(var, 1, n_bytes, file) == n_bytes;
        }break;
    }
    return false;
}

// similar to ff_var_bytes, except allocates n_bytes when in MODE_LOAD, thus requiring extra level of '&'.
static bool ff_var_bytes_alloc(FILE *file, FF_MODE mode, void **array_ptr, size_t n_bytes){
    if (mode == FF_MODE_LOAD){
        *array_ptr = FF_MALLOC(n_bytes);
        if (*array_ptr == NULL){
            return false;
        }
    }
    return ff_var_bytes(file, mode, *array_ptr, n_bytes);
}

// >string< meaning that var expects and yields a NULL terminated string.
// Requires var to point to str_len+1 allocated bytes.
// str_len is still required, because the in-file representation does not have any termination.
// If the string is not NULL terminated use bytes instead.
static bool ff_var_string(FILE *file, FF_MODE mode, char *var, size_t str_len){
    if (mode == FF_MODE_LOAD) var[str_len] = 0; // NULL terminated
    return ff_var_bytes(file, mode, var, str_len);
}

// like ff_var_string but allocates str_len+1 bytes.
static bool ff_var_string_alloc(FILE *file, FF_MODE mode, char **var_ptr, size_t str_len){
    if (mode == FF_MODE_LOAD){
        *var_ptr = (char*)FF_MALLOC(str_len+1);
        if (*var_ptr == NULL){
            return false;
        }
    }
    return ff_var_string(file, mode, *var_ptr, str_len);
}

// only in FF_MODE_LOAD
static size_t __ff_num_bytes_until(FILE *file, char terminator){
    size_t start_pos = ftell(file);
    size_t end_pos = start_pos;
    char c = 0;
    // security risk, for streams that don't end and don't yield EOF
    while ((c = fgetc(file)) != EOF){
        if (c == terminator){
            break;
        }
    }
    end_pos = ftell(file);
    // reset position in file to where it was at the call of this function
    fseek(file, start_pos, SEEK_SET);
    return end_pos - start_pos - 1; // -1 to stop before the terminator/EOF
}

// a string that ends with given terminator.
// Fails if the string contains the terminator.
static bool ff_var_string_until(FILE *file, FF_MODE mode, char *var, char terminator){
    size_t str_len = 0;
    switch (mode) {
        case FF_MODE_LOAD:{
            str_len = __ff_num_bytes_until(file, terminator);
            var[str_len] = 0; // NULL terminated
            return fread(var, 1, str_len, file) == str_len
                && fgetc(file) == terminator;
        }break;
        case FF_MODE_SAVE:{
            if (strchr(var, terminator)) return false; // string contains terminator
            str_len = strlen(var);
            return fwrite(var, 1, str_len, file) == str_len
                && fputc(terminator, file) == terminator;
        }break;
    }
    return false;
}

// like ff_var_string_until but allocates str_len+1 bytes.
static bool ff_var_string_until_alloc(FILE *file, FF_MODE mode, char **var_ptr, char terminator){
    if (mode == FF_MODE_LOAD){
        // TODO: pass this to ff_var_string_until somehow
        size_t str_len = __ff_num_bytes_until(file, terminator);
        *var_ptr = (char*)FF_MALLOC(str_len+1);
        if (*var_ptr == NULL){
            return false;
        }
    }
    return ff_var_string_until(file, mode, *var_ptr, terminator);
}

#endif // FFORMAT_IMPL
