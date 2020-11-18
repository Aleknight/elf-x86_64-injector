#include <stdlib.h>
#include <elf.h>
#include <string.h>

#include "elf-parser.h"

void setup_elf(byte *file_content)
{
    elf_header = (Elf64_Ehdr *) file_content;
}

Elf64_Addr get_entry_point(Elf64_Ehdr *elf_header)
{
    return elf_header->e_entry;
}

Elf64_Shdr *get_section_by_name(const char *section_name)
{
    Elf64_Shdr *section_table = (void *)elf_header + elf_header->e_shoff;
    Elf64_Shdr *string_table = &(section_table[elf_header->e_shstrndx]);

    for (uint16_t i = 0; i < elf_header->e_shnum; i++) {
	if ( strcmp(section_name, (void *)elf_header + string_table->sh_offset + section_table[i].sh_name) == 0 ){
	    return &(section_table[i]);    
	}
    }

    return NULL;
}

