#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>

#include "config.h"

int main(int argc, char **argv)
{
    int fd;					// File descriptor the file to infect
    struct stat *to_infect_stat = NULL;		// Structure to save the stat of the file to infect
    byte *to_infect_content = NULL;		// Buffer that contents the 
    
    // We check if the usage is correct
    if (argc != EXPECTED_ARGC) {
	fprintf(stderr, "Usage: %s <file-to-infect>", argv[0]);
	exit(EXIT_FAILURE);
    }

    // We read read the file content
    fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
	perror("open error :");
	exit(EXIT_FAILURE);
    }

    to_infect_stat = malloc(sizeof(struct stat));
    if (to_infect_stat == NULL) {
	perror("malloc error :");
	exit(EXIT_FAILURE);
    }

    if (fstat(fd, to_infect_stat)) {
	perror("fstat error :");
	exit(EXIT_FAILURE);
    }

    to_infect_content = calloc(to_infect_stat->st_size, sizeof(byte));
    if (to_infect_content == NULL) {
	perror("calloc error:");
	exit(EXIT_FAILURE);
    }

    if (read(fd, to_infect_content, to_infect_stat->st_size) != to_infect_stat->st_size) {
	fprintf(stderr, "read error");
	exit(EXIT_FAILURE);
    }

    close(fd);
    return EXIT_SUCCESS;
}
