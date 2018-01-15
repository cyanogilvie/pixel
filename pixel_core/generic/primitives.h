#ifndef _PRIMITIVES_H
#define _PRIMITIVES_H

#include "2d.h"

EXTERN_C void box(gimp_image_t *dest, int x, int y, int w, int h,
		_pel colour, int flags);
EXTERN_C void line(int x1, int y1, int x2, int y2, _pel col,
		gimp_image_t *dest);
EXTERN_C void line_aa(int x1, int y1, int x2, int y2, _pel col,
		gimp_image_t *dest);
EXTERN_C void line_aa_osa(double x1, double y1, double x2, double y2,
		_pel col, int osa, gimp_image_t *dest);
EXTERN_C void bezier(double x1, double y1, double cpx1, double cpy1,
		double cpx2, double cpy2, double x2, double y2, _pel colour,
		int osa, gimp_image_t *dest);
EXTERN_C struct pmapf* pmapf_gradient_radial(int width, int height, pelf* centre_colour, pelf* outer_colour);
EXTERN_C struct pmapf* pmapf_gradient_linear_v(int width, int height, pelf* top_colour, pelf* bottom_colour);
EXTERN_C struct pmapf* pmapf_alpha_over(struct pmapf* dest, struct pmapf* src, int xofs, int yofs);

#endif
