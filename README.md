# Description
The main idea behind fformat is, to combine the code for loading and saving of a file format into one function, that can also serve as the definition of the format.

To achieve this, all provided functions receive a mode (load or save) that changes their behaviour between reading and writing files.

# Example
```c
#define FFORMAT_IMPL
#include "fformat.h"

bool friendly_file_io(FF_MODE mode, int *number, char *path){

    FILE *file = ff_with_file(path, mode);

    // Definiton of file format. (This layout supports easy copy pasting of lines)
    bool success = true
    && ff_str_literal(file, mode, "Hello, I am a friendly file!\n") // write or assert strings
    && ff_str_literal(file, mode, "can we be friends?? <.<\n")
    && ff_str_literal(file, mode, "here's my number: \"")
    && ff_var_content(file, mode, number, sizeof(*number)) // read or write data
    && ff_str_literal(file, mode, "\" uwu\n")
    ;

    // remember to close the file.
    fclose(file);
    return success;
}
```
