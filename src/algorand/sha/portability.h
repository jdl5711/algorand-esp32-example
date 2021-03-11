/*
This code is written by kerukuro for cppcrypto library (http://cppcrypto.sourceforge.net/)
and released into public domain.
*/

#ifndef CPPCRYPTO_PORTABILITY_H
#define CPPCRYPTO_PORTABILITY_H

#include <stdint.h>
#include <stdlib.h>


extern "C" {
    //#include <stdlib.h>    
}



#define CPPCRYPTO_STATIC_ALIGN(x) __attribute__((aligned(x)))

#define swap_uint64 __builtin_bswap64
#define swap_uint32 __builtin_bswap32
#define swap_uint16 __builtin_bswap16
#define aligned_allocate calloc
#define aligned_deallocate std::free
#define FASTCALL
#define CPPCRYPTOAPI


static inline uint64_t rotater64(uint64_t x, unsigned n)
{
	return (x >> n) | (x << (64 - n));
}

static inline uint64_t rotatel64(uint64_t x, unsigned n)
{
	return (x << n) | (x >> (64 - n));
}

static inline void zero_memory(void *v, size_t n) {
	volatile unsigned char *p = (volatile unsigned char *)v;
	while (n--) {
		*p++ = 0;
	}
}
#endif



