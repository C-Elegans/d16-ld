#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include "main.h"
#include <string.h>
#include "link.h"
GArray* object_files;
int start_address = 0;
extern int setenv(const char*, const char*, int);
void print_hex(uint16_t* data, int size){
    for(int i=0;i<size;i+=2){
        printf("0x%x\n",*data);
        data++;
    }
}
int main(int argc, char** argv){
    int c;
    setenv("POSIXLY_CORRECT", "1", 1);
    opterr = 0;
    object_files = g_array_new(FALSE,FALSE, sizeof(object_file_entry));
    FILE* output = NULL;
    while(optind < argc){
        if((c=getopt(argc,argv,"o:s:"))!= -1){
            switch(c){
                case 'o':
                    output = fopen(optarg,"wb");
                    break;
                case 's':
                    start_address = strtol(optarg,NULL,0);
                    break;

            }
        }else{ // non option argument
            FILE* input = fopen(argv[optind],"rb");
            if(input == NULL){
                fprintf(stderr, "Could not open file: %s\n",argv[optind]);
                exit(1);
            }
            aout_header header;
            object_file_entry entry;
            memset(&entry,0, sizeof(object_file_entry));
            fread(&header, 1, sizeof(aout_header),input);
            if(header.a_magic != A_MAGIC){
                fprintf(stderr, "File \"%s\"is not in d16 a.out format. Perhaps you provided a binary?\nMagic Number: 0x%x",argv[optind],header.a_magic);
                exit(1);
            }
            entry.header = header;
            if(header.a_text > 0) {
                entry.text = malloc(header.a_text);
                //fseek(input, sizeof(header), SEEK_SET);
                fread(entry.text, header.a_text, 1, input);

            }
            if(header.a_data >0){
                entry.data = malloc(header.a_data);
                fread(entry.data, header.a_data, 1, input);
            }
            if(header.a_syms >0){
                entry.syms = malloc(header.a_syms);
                fread(entry.syms, header.a_syms,1,input);
            }
            if(header.a_trsize > 0){
                entry.text_relocs = malloc(header.a_trsize);
                fread(entry.text_relocs,header.a_trsize,1,input);
            }
            if(header.a_drsize >0 ){
                entry.data_relocs = malloc(header.a_drsize);
                fread(entry.data_relocs,header.a_drsize,1,input);
            }
            long pos = ftell(input);
            fseek(input,0,SEEK_END);
            long len = ftell(input)-pos;
            entry.strings = malloc(len);
            fseek(input,pos,SEEK_SET);
            fread(entry.strings,len,1,input);
            fclose(input);
            g_array_append_val(object_files,entry);


            optind++;
        }
    }
    if(output == NULL){
        fprintf(stderr,"d16-ld: No output file specified\n");
        exit(-1);
    }
    printf("Linking objects at address: %x\n",start_address);
    link_objects(object_files,output);


}
