# Description
*Header-only library conforming to C99.*

The main idea behind `fformat` is to combine the code for loading and saving of a file format into one function, which also serves as the definition of the format.

To achieve this, all provided functions receive a mode (load or save) that changes their behaviour from reading to writing files.

# Example
```c
#define FFORMAT_IMPL
#include "fformat.h"

bool friendly_file_io(FF_MODE mode, int32_t *number, char *path){

    FILE *file = ff_with_file(path, mode);

    // Definition of file format. (This code formatting supports easy copy pasting of lines)
    bool success = true
    && ff_lit_string(file, mode, "Hello, I am a friendly file!\n") // write or assert strings
    && ff_lit_string(file, mode, "can we be friends?? <.<\n")
    && ff_lit_string(file, mode, "here's my number: \"")
    && ff_var_bytes(file, mode, number, sizeof(*number)) // read or write data
    && ff_lit_string(file, mode, "\" uwu\n")
    ;

    // remember to close the file.
    fclose(file);
    return success;
}
```
use `make` to build the example programs (or compile them yourself, they are only 1 c-file each, no dependencies).

# Features
- declarative file format through function statements.
- unification of save & load user code

- supports:
    - String literals (boilerplate text)
    - variable and array R/W (i.e. bytes)
    - string R/W (NULL-terminated bytes)
    - in-file terminated strings

## Anti-Features
- pattern matching
- schemas

# User Guide
See the description in `fformat.h` for more details.

## Function names
- The function names are a composition of different keywords:
- *prefix*: `lit_` functions assert that their data exists in the file. `var_` functions save/load data from/to the var pointer.
- *content*: `string` means, that the byte array is NULL-terminated in working memory (not necessarily in file).
  It is a convenience variant of `bytes`, which is not NULL-terminated.
- All functions have an `_alloc` variant that allocates memory on load. (allocator can be overwritten)
- Some Functions are slower than others:
    - Seeking functions like `_until` are usually slower than saving the size in the file format explicitly.
    - Parsing functions (if they exist yet) are also slow.
    - These functions only exist to improve human readability and to make things prettier to look at.
