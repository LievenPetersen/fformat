#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>

#define FFORMAT_IMPL
#include "fformat.h"


// Function that defines a file format.
// It can both load and save the the file format, depending on the given mode.
// This is not limited to a single struct or static file sizes, you can do whatever you like.
bool friendly_file_io(FF_MODE mode, int32_t *number, char *path){

    FILE *file = ff_open(path, mode);

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

int main(){
    int32_t incoming_data = 12345678;
    bool save_success = friendly_file_io(FF_MODE_SAVE, &incoming_data, "test.friendly");
    assert(save_success);

    int32_t loaded_data = 0;
    bool load_success = friendly_file_io(FF_MODE_LOAD, &loaded_data, "test.friendly");
    assert(load_success);

    // check if both structs have the same content
    if (loaded_data == incoming_data){
        printf("number stored & loaded successfully!!\n");
    } else {
        printf("Oh no! :´( numbers are different!!\n");
    }
    return 0;
}
