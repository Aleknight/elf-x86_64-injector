#ifndef _INSERTION_H_
#define _INSERTION_H_

#include "config.h"

/* ---------------- Functio definitions ---------------- */
void insert(byte *dest, byte *src, uint64_t dest_size, uint16_t size);
void add_jump(byte **payload_addr, uint16_t *size, Elf64_Addr new_entry, Elf64_Addr old_entry);
#endif
