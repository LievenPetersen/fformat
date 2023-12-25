#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

#define FFORMAT_IMPL
#include "fformat.h"


typedef struct{
    int a;
    int b;
}example_t;

// Function that defines a file format.
// It can both load and save the the file format, depending on the given direction.
// this is not limited to a single struct or static file sizes, you can do whatever you want.
bool friendly_file_io(ACTION_ENUM direction, example_t *subject, char *path){
    FILE *file = with_file(path, direction);

    // Definiton of file contents. (This layout supports easy copy pasting of lines)
    bool success = true
    && str_literal(file, direction, "Hello, I am a friendly file!\n")
    && str_literal(file, direction, "can we be friends?? <.<\n")
    && str_literal(file, direction, "here's my number: \"")
    && var_content(file, direction, &subject->a, sizeof(subject->a))
    && var_content(file, direction, &subject->b, sizeof(subject->b))
    && str_literal(file, direction, "\" uwu\n")
    ;

    // remember to close the file.
    fclose(file);
    return success;
}

int main(){
    example_t incoming_data = {.a = 1, .b = -12};
    bool save_success = friendly_file_io(ACTION_SAVE, &incoming_data, "test.friendly");
    assert(save_success);

    example_t loaded_data = {0};
    bool load_success = friendly_file_io(ACTION_LOAD, &loaded_data, "test.friendly");
    assert(load_success);

    // check if both structs have the same content
    if (loaded_data.a == incoming_data.a && loaded_data.b == incoming_data.b){
        printf("struct loaded successfully!!\n");
    } else {
        printf("Oh no! :´( structs are different!!\n");
    }
    return 0;
}
