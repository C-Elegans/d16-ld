//
// Created by Michael Nolan on 7/28/16.
//

#include "link.h"
#include "main.h"
#include <stdlib.h>

void link_objects(GArray* objects, FILE* output){
    size_t output_size = 0;
    for(int i=0;i<objects->len;i++){
        object_file_entry entry = g_array_index(objects, object_file_entry,i);
        output_size += entry.header.a_text;
        output_size += entry.header.a_data;
    }
    uint16_t *output_buffer = malloc(output_size);
}