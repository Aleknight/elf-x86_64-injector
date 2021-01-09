#include <stdlib.h>
#include <elf.h>
#include <string.h>

#include "elf-parser.h"
#include "log.h"

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

Elf64_Phdr *get_exe_segment(void) 
{
    Elf64_Phdr *program_headers;

    if (elf_header->e_phoff == 0) {
	ERROR("No programm header for this one", NULL);
    }
    
    program_headers = (Elf64_Phdr *)((void *)elf_header + elf_header->e_phoff);
    for (uint64_t i = 0; i < elf_header->e_phnum; i++) {
	if ((program_headers[i].p_flags & PF_X) && (program_headers[i].p_type & PT_LOAD)) {
	    SUCCESS("Executable segment found ! Virtual @ %p", program_headers[i].p_vaddr);
	    return &(program_headers[i]);
	}
    }

    return NULL;
}
