#ifndef _ELF_PARSER_H_
#define _ELF_PARSER_H_

#include <elf.h>
#include <stdbool.h>

#include "config.h"

/* --------------- Global vars -------------- */
extern Elf64_Ehdr *elf_header;

/* ---------------- Functions --------------- */

void setup_elf(byte *file_content);

Elf64_Addr get_entry_point();

void set_entry_point(Elf64_Addr vaddr);

Elf64_Shdr *get_section_by_name(const char *section_name);

Elf64_Phdr *get_exe_segment(void);

bool section_in_segment(Elf64_Shdr *section, Elf64_Phdr *segment);

void add_offset(uint64_t base, uint16_t value);

void add_jump(byte **payload_addr, uint16_t *size, Elf64_Addr new_entry, Elf64_Addr old_entry);

void update_symbol(const char *name, int64_t value);

void update_section(const char *name, int64_t value);

void update_dynsym(Elf64_Xword symbol, int64_t value);

void check_payload_place(uint64_t payload_size);
#endif /* _ELF_PARSER_H_ */
