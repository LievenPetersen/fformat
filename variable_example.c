#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stdint.h"

#define FFORMAT_IMPL
#include "fformat.h"


typedef struct{
    char *name; // NULL terminated
    char *desc; // NULL terminated
    uint32_t width;
    uint32_t height;
    int32_t *data;
}map_t;

// TODO custom allocator and free?
bool map_io(char *path, FF_MODE mode, map_t *map){
    FILE *file = ff_open(path, mode);

    // variable to store the strlen in front of the string
    size_t name_len = mode == FF_MODE_SAVE ? strlen(map->name) : 0;

    bool success = true
    && ff_lit_string(file, mode, "Map v1.3\n")
    && ff_lit_string(file, mode, "Name (prefix len): ")
    && ff_var_bytes(file, mode, &name_len, sizeof(name_len))
    ;
    // TODO support quoted string?

    success// it's important to keep the success && .. chain going, to ensure an abort on first error.
    && ff_var_string_alloc(file, mode, &map->name, name_len)
    && ff_lit_string(file, mode, "\nDescription (LF terminated): ")
    && ff_var_string_until_alloc(file, mode, &map->desc, '\n')
    && ff_lit_string(file, mode, "Width: ")
    // TODO support formatting and parsing? scanf is unsafe? maybe own scanf + parsing?
    && ff_var_bytes(file, mode, &map->width, sizeof(map->width))
    && ff_lit_string(file, mode, "\nHeight: ")
    && ff_var_bytes(file, mode, &map->height, sizeof(map->height))
    && ff_lit_string(file, mode, "\nData:\n")
    && ff_var_bytes_alloc(file, mode, (void**)&map->data, map->height * map->width * sizeof(*map->data))
    && ff_lit_string(file, mode, "\n")
    ;

    fclose(file);
    return success;
}

int main(){
    map_t map = {
        .name = "nice map",
        .desc = "it's a nice map of wonky proportions!",
        .width = 23, .height = 7,
    };
    map.data = malloc(sizeof(map.data) * map.width * map.height);

    for (size_t i = 0; i < map.width * map.height; i++){
        map.data[i] = i*((i % 3) - 1);
    }

    // save
    assert(map_io("beautiful.map", FF_MODE_SAVE, &map));

    // load
    map_t loaded_map = {0};
    assert(map_io("beautiful.map", FF_MODE_LOAD, &loaded_map));

    // check equality
    assert(strcmp(map.name, loaded_map.name) == 0);
    assert(strcmp(map.desc, loaded_map.desc) == 0);
    assert(map.width == loaded_map.width);
    assert(map.height == loaded_map.height);
    assert(memcmp(map.data, loaded_map.data, map.width * map.height * sizeof(*map.data)) == 0);

    // if you did not define FF_MALLOC, you may assume that the stdlib.h malloc/free are (and can be) used
    FF_FREE(loaded_map.name);
    FF_FREE(loaded_map.desc);
    FF_FREE(loaded_map.data);
    FF_FREE(map.data);

    // no assertions failed
    printf("success\n");
}

