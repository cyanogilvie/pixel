#include "pixel.h"
#include "2d_lookups.h"

void quadshade(double x, double y, _pel colour, int osa, int flags, gimp_image_t *dest)
{
	//double	fx, fy;
	//int		ox, oy;
	_pel	*p;

//	fx = x % 1;
//	fy = y % 1;
	//fx = x - (int)(x);
	//fy = y - (int)(y);

	/*
	ox = (fx < 0.5) ? -1 : 1;
	oy = (fy < 0.5) ? -dest->width : dest->width;
	*/

	p = dest->pixel_data + (int)(x) + (int)(y) * dest->width;

	// Sigh.  At some point do magic here
	P_ALPHA(p, (&colour));
}


