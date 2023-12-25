/*
MIT License

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
SOFTWARE.


* Header-only-library for simple definition of custom file formats.
*
* See example.c on how it is intended to be used.
*
* Config:
* #define FFORMAT_IMPL  // to access function definitions
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
    MODE_LOAD,
    MODE_SAVE,
}FF_MODE;

// open a file in the indicated mode (needs to be closed manually)
static FILE *ff_open(char *path, FF_MODE mode);
// define the next section to contain the given string. str needs to be NULL terminated.
// NULL terminator will not be written to file.
static bool ff_str_literal(FILE *file, FF_MODE mode, char *str);
// pointer to a memory location of n_bytes size. Data is written/read as raw bytes.
static bool ff_var_content(FILE *file, FF_MODE mode, void *var, size_t n_bytes);

#endif // __FFORMAT_H

// unlock definitons
#ifdef FFORMAT_IMPL

// open a file in the indicated mode (needs to be closed manually)
static FILE *ff_open(char *path, FF_MODE mode){
    switch(mode){
        case MODE_LOAD: return fopen(path, "rb");
        case MODE_SAVE: return fopen(path, "wb");
    }
    return NULL;
}

// define the next section to contain the given string. str needs to be NULL terminated.
// NULL terminator will not be written to file.
static bool ff_str_literal(FILE *file, FF_MODE mode, char *str){
    size_t str_len = strlen(str);
    switch (mode) {
        case MODE_LOAD: {
            char buf[str_len];
            return fread(buf, 1, str_len, file) == str_len 
                && strncmp(str, buf, str_len) == 0;
        }break;
        case MODE_SAVE: {
            return fwrite(str, 1, str_len, file) == str_len;
        }break;
    }
    return false;
}

// pointer to a memory location of n_bytes size. Data is written/read as raw bytes.
static bool ff_var_content(FILE *file, FF_MODE mode, void *var, size_t n_bytes){
    switch (mode) {
        case MODE_LOAD: {
            return fread(var, 1, n_bytes, file) == n_bytes;
        }break;
        case MODE_SAVE: {
            return fwrite(var, 1, n_bytes, file) == n_bytes;
        }break;
    }
    return false;
}

#endif // FFORMAT_IMPL
