#include <stdint.h>
#include <elf.h>
#include <stdlib.h>
#include <log.h>

#include "elf-parser.h"

/*
 * Insert the _dest_ array into the ELF file
 * _dest_ in an executable section
 */
void insert(byte *dest, byte *src, uint16_t size) {

    byte *final_product = NULL;

    setup_elf(dest);
    
    /* First we find the section .text */
    Elf64_Shdr *text = get_section_by_name(".text");
    if (text == NULL) {
	ERROR("Can't find the section .text", NULL);
    }

    /* Then we find the segment that contains the text section */
    for (Elf64_Phdr *seg = get_exe_segment(); seg != NULL; seg = get_exe_segment()) {
	if (section_in_segment(text, seg)) {
	    break;
	}
    }

    /* Now we update the size of this segment/sections */

    /* Now we update the entry point */

    /* We allocate a new buffer for the new executable */

    /* We fill this new binary buffer */
}
