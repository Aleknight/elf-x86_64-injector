#ifndef _ELF_PARSER_H_
#define _ELF_PARSER_H_

#include <elf.h>

#include "config.h"

/* --------------- Global vars -------------- */
static Elf64_Ehdr *elf_header;

/* ---------------- Functions --------------- */
void setup_elf(byte *file_content);

#endif /* _ELF_PARSER_H_ */
