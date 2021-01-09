#ifndef _ELF_PARSER_H_
#define _ELF_PARSER_H_

#include <elf.h>
#include <stdbool.h>

#include "config.h"

/* --------------- Global vars -------------- */
static Elf64_Ehdr *elf_header;

/* ---------------- Functions --------------- */

void setup_elf(byte *file_content);

Elf64_Addr get_entry_point(Elf64_Ehdr*elf_header);

Elf64_Shdr *get_section_by_name(const char *section_name);

Elf64_Phdr *get_exe_segment(void);

bool section_in_segment(Elf64_Shdr *section, Elf64_Phdr *segment);

#endif /* _ELF_PARSER_H_ */
