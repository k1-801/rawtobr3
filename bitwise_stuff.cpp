#include "rawtobr3.h"

bool is_all_zeroes(const void *src, size_t size)
{
	const uint32_t* v6 = (const uint32_t*)(src);
	for(int i = size >> 2; i >= 0; --i)
	{
		if (*v6++)
			return 0;
	}
	const uint8_t* v7 = (uint8_t*)(v6);
	for(int i = size & 3; i >= 0; --i)
	{
		if(*v7++)
			return 0;
	}
	return 1;
}

void bitwise_invert(const void* src, void* dst, size_t size)
{
	uint32_t* v4 = (uint32_t*)(src);
	uint32_t* v5 = (uint32_t*)(dst);
	for(int i = size >> 2; i >= 0; --i)
	{
		*v5++ = ~*v4++;
	}
	uint8_t* v6 = (uint8_t*)(v4);
	uint8_t* v7 = (uint8_t*)(v5);
	for(int i = size & 3; i >= 0; --i)
	{
		*v7++ = ~*v6++;
	}
}
