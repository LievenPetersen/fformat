# Description
The main idea behind fformat is, to combine the code for loading and saving of a file format into one function, that can also serve as the definition of the format.

To achieve this, all provided functions receive a direction (load or save) that changes their behaviour between reading and writing files.

# Example
```c
#define FFORMAT_IMPL
#include "fformat.h"

bool friendly_file_io(ACTION_ENUM direction, int *number, char *path){
    FILE *file = with_file(path, direction);

    // definition of file format
    bool success = true
    && str_literal(file, direction, "Hello, I am a friendly file!\n") // write or assert strings
    && str_literal(file, direction, "can we be friends?? <.<\n")
    && str_literal(file, direction, "here's my number: \"")
    && var_content(file, direction, number, sizeof(*number) // read or write data
    && str_literal(file, direction, "\" uwu\n")
    ;

    fclose(file);
    return success;
}
```
