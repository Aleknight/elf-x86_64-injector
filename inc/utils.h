#ifndef _UTILS_H_
#define _UTILS_H_

#define G_ADD(value, base, bonus) \
    value += (value > base ? bonus : 0)

/* ---------------- Function definitions ---------------- */

byte *read_file(const char *filename, uint64_t *file_size);
void write_file(const char *filename, byte *buffer, uint64_t size);

#endif /* _UTILS_H_ */
