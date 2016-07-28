//
// Created by Michael Nolan on 7/28/16.
//
#include <glib-2.0/glib.h>
#include <stdint.h>
#ifndef D16_LD_MAIN_H
#define D16_LD_MAIN_H


typedef struct _aout_header {
    uint32_t a_magic;
    uint32_t a_text;
    uint32_t a_data;
    uint32_t a_bss;
    uint32_t a_syms;
    uint32_t a_entry;
    uint32_t a_trsize;
    uint32_t a_drsize;
}aout_header;

typedef struct _reloc_entry {
    unsigned int address;
    unsigned int index:24;
    unsigned int pc_rel:1;
    unsigned int length:2;
    unsigned int extern_entry:1;
    unsigned int spare:4;
} a_reloc_entry;

typedef struct _symbol_entry{
    uint32_t name_offset;
    uint8_t type;
    uint8_t spare;
    uint16_t debug_info;
    uint32_t value;
} a_symbol_entry;
typedef struct {
    aout_header header;
    void* text;
    void* data;
    int text_start_offset;
    int data_start_offset;
    a_symbol_entry* syms;
    a_reloc_entry* text_relocs;
    a_reloc_entry* data_relocs;
    char* strings;
} object_file_entry;
#endif //D16_LD_MAIN_H
