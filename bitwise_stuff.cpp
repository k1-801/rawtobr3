#include "rawtobr3.h"

bool is_all_zeroes(const void *src, size_t size)
{
	const uint32_t* sp = (const uint32_t*)(src);
	for(int i = size >> 2; i >= 0; --i)
	{
		if (*sp++)
			return 0;
	}
	const uint8_t* sp2 = (uint8_t*)(sp);
	for(int i = size & 3; i >= 0; --i)
	{
		if(*sp2++)
			return 0;
	}
	return 1;
}

void bitwise_invert(const void* src, void* dst, size_t size)
{
	uint32_t* sp = (uint32_t*)(src);
	uint32_t* dp = (uint32_t*)(dst);
	for(int i = size >> 2; i >= 0; --i)
	{
		*dp++ = ~*sp++;
	}
	uint8_t* sp2 = (uint8_t*)(sp);
	uint8_t* dp2 = (uint8_t*)(dp);
	for(int i = size & 3; i >= 0; --i)
	{
		*dp2++ = ~*sp2++;
	}
}
