#include "2d.h"


void asm_pelset(_pel *dest, _pel colour, uint32 count)
{
	int		i = count;
	_pel	*p;

	while (i--)
		*(p++) = colour;
}

