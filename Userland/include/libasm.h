#ifndef LIBASM_H
#define LIBASM_H

#include <stdint.h>

/*
 * Wrapper function for int 0x80
 */
int64_t _int80(uint64_t syscallID, int64_t p1, int64_t p2, int64_t p3);



#endif