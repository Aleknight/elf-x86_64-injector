#include <stdlib.h>
#include <elf.h>
#include <string.h>
#include <stdbool.h>

#include "elf-parser.h"
#include "log.h"
#include "utils.h"

Elf64_Ehdr *elf_header;

void setup_elf(byte *file_content)
{
    elf_header = (Elf64_Ehdr *) file_content;
}

Elf64_Addr get_entry_point() {
    return elf_header->e_entry;
}

void set_entry_point(Elf64_Addr vaddr) {
    elf_header->e_entry = vaddr;
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

void add_offset(uint64_t base, uint16_t value) {
    Elf64_Phdr *segment_table = (Elf64_Phdr *)((void *)elf_header + elf_header->e_phoff);
    Elf64_Half segment_table_size = elf_header->e_phnum;
    Elf64_Shdr *section_table = (Elf64_Shdr *)((void *)elf_header + elf_header->e_shoff);
    Elf64_Half section_table_size = elf_header->e_shnum;

    G_ADD(elf_header->e_phoff, base, value);
    G_ADD(elf_header->e_shoff, base, value);

    for (Elf64_Half i = 0; i < segment_table_size; i++) {
	G_ADD(segment_table[i].p_offset, base, value);
	G_ADD(segment_table[i].p_vaddr, base, value);
    }
    for (Elf64_Half i = 0; i < section_table_size; i++) {
	G_ADD(section_table[i].sh_offset, base, value);
	G_ADD(section_table[i].sh_addr, base, value);
    }
}

void update_symbol(const char *name, int64_t value) {
    Elf64_Shdr *symtab = get_section_by_name(".symtab");
    Elf64_Shdr *string_table = get_section_by_name(".strtab");
    Elf64_Sym *symtable = (Elf64_Sym *)(((byte *)elf_header) + symtab->sh_offset);
    for (uint64_t i = 0; i < (symtab->sh_size / sizeof(Elf64_Sym)); i++) {
	if (strcmp(name, (byte *)elf_header + string_table->sh_offset + symtable[i].st_name) == 0) {
	    symtable[i].st_value += value;
	}
    }
}

void update_section(const char *name, int64_t value) {
    Elf64_Shdr *section_table = (void *)elf_header + elf_header->e_shoff;
    Elf64_Shdr *string_table = get_section_by_name(".shstrtab");
    for (uint64_t i = 0; i < elf_header->e_shnum; i++) {
	if (strcmp(name, (byte *)elf_header + string_table->sh_offset + section_table[i].sh_name) == 0) {
	    section_table[i].sh_offset += value;
	    section_table[i].sh_addr += value;
	}
    }
}

void update_dynsym(Elf64_Xword symbol, int64_t value) {
    Elf64_Shdr *dyn_table = get_section_by_name(".dynamic");
    Elf64_Dyn *table = (Elf64_Dyn *)((byte *)elf_header + dyn_table->sh_offset);
    while (table->d_tag != DT_NULL) {
	if (table->d_tag == symbol) {
	    table->d_un.d_ptr += value;
	    return;
	}
	table += 1;
    }
}
