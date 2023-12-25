#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

#define FILESPEC_IMPL
#include "filespec.h"


typedef struct{
    int a;
    int b;
}example_t;

// function that can both load and save the the file format, depending on the direction.
// this is not limited to a single struct or static file sizes, you can do whatever you want.
bool friendly_file_format(example_t *subject, char *path, ACTION_ENUM direction){
    FILE *file = with_file(path, direction);

    // Definiton of file contents. (This layout supports easy copy pasting of lines)
    bool success = true
    && str_literal(file, direction, "Hello, I am a friendly file!\n")
    && str_literal(file, direction, "can we be friends?? <.<\n")
    && str_literal(file, direction, "here's my number: \"")
    && var_content(file, direction, &subject->a, sizeof(subject->a))
    && var_content(file, direction, &subject->b, sizeof(subject->b))
    && str_literal(file, direction, "\" uwu")
    && str_literal(file, direction, "\n")
    ;

    // remember to close the file.
    fclose(file);
    return success;
}

int main(){
    example_t incoming_data = {.a = 1, .b = -12};
    bool ma_success = friendly_file_format(&incoming_data, "test.friendly", ACTION_SAVE);
    assert(ma_success);

    example_t loaded_data = {0};
    bool ma_loading = friendly_file_format(&loaded_data, "test.friendly", ACTION_LOAD);
    assert(ma_loading);

    // check if both structs have the same content
    if (memcmp(&incoming_data, &loaded_data, sizeof(incoming_data)) == 0){
        printf("structs are equal!!\n");
    } else {
        printf("Oh no! :´( structs are different!!\n");
    }
    return 0;
}
