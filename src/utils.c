#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <log.h>

#include "config.h"
#include "utils.h"

/*
 * Read the content of the file with _filename_
 * and return the a byte array of this content
 * and the size if the caller need it
 */
byte *read_file(const char *filename, uint64_t *file_size) {
    int fd;
    struct stat file_stat;
    byte *file_content;

    /* Open the file */
    fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("open error:");
        exit(EXIT_FAILURE);
    }

    /* Get the stat of the opened file */
    if (fstat(fd, &file_stat)) {
        perror("fstat error :");
        exit(EXIT_FAILURE);
    }

    /* Alloc the space for the content of the file */
    file_content = calloc(file_stat.st_size, sizeof(byte));
    if (file_content == NULL) {
        perror("calloc error:");
        exit(EXIT_FAILURE);
    }

    /* Read the file content */
    if (read(fd, file_content, file_stat.st_size) != file_stat.st_size) {
        ERROR("Failed to read the full content of the file %s\n", filename);
    }

    close(fd);
    
    /* We return two information */
    if (file_size) {
        *file_size = file_stat.st_size;
    }
    return file_content;
}

/*
 * Write the _size_ first bytes of _buffer_
 * in the _filename_ file
 */
void write_file(const char *filename, byte *buffer, uint64_t size) {

    int fd = open(filename, O_RDWR | O_CREAT, S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR | S_IXUSR);
    if (fd == -1) {
        perror("open error:");
        exit(EXIT_FAILURE);
    }
    if (write(fd, buffer, size) != size) {
        ERROR("Failed to write in the file %s\n", filename);
    }
}
