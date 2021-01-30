#include <stdlib.h>
#include <elf.h>
#include <string.h>
#include <stdbool.h>

#include "elf-parser.h"
#include "log.h"
#include "utils.h"

Elf64_Ehdr *elf_header;
Elf64_Shdr *section_table;
Elf64_Shdr *string_table_section;
Elf64_Shdr *dyn_section;
Elf64_Shdr *text_section;

void setup_elf(byte *file_content)
{
    elf_header = (Elf64_Ehdr *) file_content;
    section_table = (Elf64_Shdr *)(file_content + elf_header->e_shoff);
    string_table_section = &(section_table[elf_header->e_shstrndx]);
    dyn_section = get_section_by_name(".dynamic");
    text_section = get_section_by_name(".text");
}

Elf64_Addr get_entry_point() {
    return elf_header->e_entry;
}

void set_entry_point(Elf64_Addr vaddr) {
    elf_header->e_entry = vaddr;
}

Elf64_Shdr *get_section_by_name(const char *section_name)
{
    for (uint16_t i = 0; i < elf_header->e_shnum; i++) {
	if ( strcmp(section_name, (void *)elf_header + string_table_section->sh_offset + section_table[i].sh_name) == 0 ){
	    return &(section_table[i]);
	}
    }
    return NULL;
}

Elf64_Phdr *get_exe_segment(void) 
{
    Elf64_Phdr *program_headers = (Elf64_Phdr *)((void *)elf_header + elf_header->e_phoff);
    static uint8_t i = 0;
    if (elf_header->e_phoff == 0) {
	ERROR("No programm header for this one", NULL);
    }
    
    for (; i < elf_header->e_phnum; i++) {
	if ((program_headers[i].p_flags & PF_X) && (program_headers[i].p_type & PT_LOAD)) {
	    SUCCESS("Executable segment found ! Virtual @ %p", program_headers[i].p_vaddr);
	    return &(program_headers[i]);
	}
    }

    return NULL;
}

bool section_in_segment(Elf64_Shdr *section, Elf64_Phdr *segment) {
    byte *segment_base = (byte *)elf_header + segment->p_offset;
    byte *section_base = (byte *)elf_header + section->sh_offset;
    return segment_base <= section_base && section_base <= segment_base + segment->p_filesz;
}

void update_symbol(const char *name, int64_t value) {
    Elf64_Shdr *symtab = get_section_by_name(".symtab");
    if (!symtab) {
	fprintf(stderr, "No symtab\n");
	return;
    }
    Elf64_Shdr *string_table = get_section_by_name(".strtab");
    Elf64_Sym *symtable = (Elf64_Sym *)(((byte *)elf_header) + symtab->sh_offset);
    for (uint64_t i = 0; i < (symtab->sh_size / sizeof(Elf64_Sym)); i++) {
	if (strcmp(name, (byte *)elf_header + string_table->sh_offset + symtable[i].st_name) == 0) {
	    symtable[i].st_value += value;
	}
    }
}

void update_section(const char *name, int64_t value) {
    Elf64_Shdr *target_section = get_section_by_name(name);
    target_section->sh_offset += value;
    target_section->sh_addr += value;
}

void update_dynsym(Elf64_Xword symbol, int64_t value) {
    Elf64_Dyn *table = (Elf64_Dyn *)((byte *)elf_header + dyn_section->sh_offset);
    while (table->d_tag != DT_NULL) {
	if (table->d_tag == symbol) {
	    table->d_un.d_ptr += value;
	    return;
	}
	table += 1;
    }
}

void check_payload_place(uint64_t payload_size) {
    uint64_t available_place = (PAGE_SIZE - (text_section->sh_size & (PAGE_SIZE - 1)));
    if (payload_size > available_place) {
	ERROR("The payload is too big to be insert. Payload size : 0x%x - Available place 0x%x", payload_size, available_place);
    }
}
