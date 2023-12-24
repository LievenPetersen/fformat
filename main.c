#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

typedef enum{
    ACTION_LOAD,
    ACTION_SAVE,
}ACTION_t;

typedef struct{
    int a;
    int b;
}ex_t;

FILE *with_file(char *path, ACTION_t direction){
    switch(direction){
        case ACTION_LOAD: return fopen(path, "rb");
        case ACTION_SAVE: return fopen(path, "wb");
    }
    return NULL;
}

bool str_literal(FILE *file, ACTION_t direction, char *str){
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

bool var_content(FILE *file, ACTION_t direction, void *var, size_t n_bytes){
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

bool example_spec(ex_t *thing, char *path, ACTION_t direction){
    FILE *file = with_file(path, direction);

    bool success = true
    && str_literal(file, direction, "Hello, I am a friendly file!\n")
    && str_literal(file, direction, "can we be friends?? <.<\n")
    && str_literal(file, direction, "here's my number: \"")
    && var_content(file, direction, &thing->a, sizeof(thing->a))
    && var_content(file, direction, &thing->b, sizeof(thing->b))
    && str_literal(file, direction, "\" uwu")
    && str_literal(file, direction, "\n")
    ;

    fclose(file);
    return success;
}

int main(){
    ex_t ex = {.a = 1, .b = -12};
    bool ma_success = example_spec(&ex, "hey", ACTION_SAVE);

    printf("saving: ");
    if (ma_success){
        printf("success!\n");
    } else {
        printf("error! :c\n");
    }
    ex_t fx = {0};
    bool ma_loading = example_spec(&fx, "hey", ACTION_LOAD);
    printf("loading: ");
    if (ma_loading){
        printf("success!\n");
    } else {
        printf("error! :c\n");
    }

    if (memcmp(&ex, &fx, sizeof(ex)) == 0){
        printf("structs are equal!!\n");
    } else {
        printf("Oh no! :´( structs are different!!\n");
    }
    return 0;
}
