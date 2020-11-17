#include <stdlib.h>
#include <elf.h>

#include "elf-parser.h"

void setup_elf(byte *file_content)
{
    elf_header = (Elf64_Ehdr *) file_content;
}
