#include "all.h"


void asm_pelset(_pel *dest, _pel colour, uint32 count)
{
	int		i = count;
	_pel	*p = dest;

	while (i--)
		*(p++) = colour;
}

