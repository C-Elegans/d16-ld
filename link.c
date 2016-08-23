//
// Created by Michael Nolan on 7/28/16.
//

#include "link.h"
#include "main.h"
#include <stdlib.h>
#include <string.h>
GHashTable* symbol_table = NULL;
void add_symbols(object_file_entry* entry){
    const a_symbol_entry* symbs = entry->syms;
    for(int i=0; i<entry->header.a_syms/ sizeof(a_symbol_entry);i++){
        a_symbol_entry* symb = malloc(sizeof(a_symbol_entry));
        memcpy(symb,symbs, sizeof(a_symbol_entry));
        symb->value += entry->text_start_offset;
        int str_offset = symb ->name_offset;
        //printf("Adding symbol %s\n",entry->strings + str_offset);
        g_hash_table_insert(symbol_table,entry->strings+str_offset,symb);
        symbs++;
    }
}
int resolve_text_relocations(object_file_entry* entry, uint16_t* buffer){
    int ret = 0;
    const a_reloc_entry * relocs = entry->text_relocs;
    for(int i=0;i<entry->header.a_trsize/ sizeof(a_reloc_entry);i++){
        char* reloc_str = relocs->index + entry->strings;
        int reloc_addr = relocs->address+entry->text_start_offset;
#ifdef DEBUG
        printf("Resolving relocation for %s at 0x%x\n",reloc_str,reloc_addr);
#endif
        a_symbol_entry* symb = g_hash_table_lookup(symbol_table,reloc_str);
        if(symb == NULL){
            fprintf(stderr,"Undefined symbols: %s\n",reloc_str);
            ret = -1;
        }else{
            uint16_t addr = (uint16_t)symb->value;
            *(buffer+reloc_addr/2+1) = addr;
        }
        relocs++;
    }
    return ret;
}
void link_objects(GArray* objects, FILE* output){
    symbol_table = g_hash_table_new(g_str_hash,g_str_equal);
    size_t output_size = 0;
    for(int i=0;i<objects->len;i++){
        object_file_entry entry = g_array_index(objects, object_file_entry,i);
        output_size += entry.header.a_text;
        output_size += entry.header.a_data;
    }
    uint16_t *output_buffer = malloc(output_size);
    int index = 0;
    //copy .text
    for(int i=0;i<objects->len;i++){
        object_file_entry* entry = &g_array_index(objects, object_file_entry,i);
        entry->text_start_offset = index;
        memcpy(output_buffer+index/2,entry->text,entry->header.a_text);
        index += entry->header.a_text;
    }
    for(int i=0;i<objects->len;i++){
        object_file_entry* entry = &g_array_index(objects, object_file_entry,i);
        entry->data_start_offset = index;
        memcpy(output_buffer+index/2,entry->data,entry->header.a_data);
        index += entry->header.a_data;
    }


    for(int i=0;i<objects->len;i++) {
        object_file_entry *entry = &g_array_index(objects, object_file_entry, i);
        add_symbols(entry);
    }
    int status = 0;
    for(int i=0;i<objects->len;i++) {
        object_file_entry *entry = &g_array_index(objects, object_file_entry, i);
        int ret =resolve_text_relocations(entry,output_buffer);
        if(ret == -1){
            status = -1;
        }
    }
    if(status != 0){
        exit(-1);
    }
    fwrite(output_buffer,output_size,1,output);
    printf("Program length: %lu bytes\n",output_size);
}