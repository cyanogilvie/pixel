#include "pixel.h"
#include "2d_lookups.h"


void box(dest, x, y, w, h, colour, flags)
	gimp_image_t	*dest;
	int				x, y, w, h;
	_pel			colour;
	int				flags;
{
	uint32		c;
	int			l;
	_pel		*p;
	
	c = colour.c;
	p = dest->pixel_data + y*dest->width + x;
	
	for (l=0; l<h; l++, p+=dest->width)
		row_set(p, colour, w, flags);
}


/* Danger, Will Robinson!  We do no clipping here, so trying to draw outside
   the buffer will merrily segfault! */
void line(int x1, int y1, int x2, int y2, _pel col, gimp_image_t *dest)
{
    int         l;
    int         hold;
    _pel		*p;
    int         delta_inc, acc_inc = 0x7fff;
    int         delta_x, delta_y;

    if ( abs(y2-y1) > abs(x2-x1) ) {
        if (y1 > y2) {
            hold = y1;
            y1 = y2;
            y2 = hold;
            hold = x1;
            x1 = x2;
            x2 = hold;
        }
    } else {
        if (x1 > x2) {
            hold = y1;
            y1 = y2;
            y2 = hold;
            hold = x1;
            x1 = x2;
            x2 = hold;
        }
    }

    delta_x = x2-x1;
    delta_y = y2-y1;

    p = dest->pixel_data + (x1 + y1*dest->width);

    if ( abs(delta_y) > abs(delta_x) ) {
        if (delta_x==0) {
            // decays to a VLine
            for (l=y1; l<=y2; l++) {
                *p = col;
                p += dest->width;
            }
        } else {
            // loop for every y
            delta_inc = ((delta_x) * 0x10000) / delta_y;
            for (l=y1; l<=y2; l++) {
                *p = col;
                acc_inc += delta_inc;
                p += acc_inc >> 16;
                p += dest->width;
                acc_inc &= 0xffff;
            }
        }
    } else {
        if (delta_y==0) {
            // decays to an HLine
            for (l=x1; l<=x2; l++)
                *p++ = col;
        } else {
            // Loop for every x
            delta_inc = ((delta_y) * 0x10000) / delta_x;
            for (l=x1; l<=x2; l++) {
                *p = col;
                acc_inc += delta_inc;
                p += (acc_inc >> 16) * dest->width;
                p++;
                acc_inc &= 0xffff;
            }
        }
    }
}


/* Danger, Will Robinson!  We do no clipping here, so trying to draw outside
   the buffer will merrily segfault! */
void line_aa(int x1, int y1, int x2, int y2, _pel col, gimp_image_t *dest)
{
    int         l;
    int         hold;
    _pel		*p;
    int         delta_inc, acc_inc = 0x7fff;
    int         delta_x, delta_y;
	_pel		pel;

    if ( abs(y2-y1) > abs(x2-x1) ) {
        if (y1 > y2) {
            hold = y1;
            y1 = y2;
            y2 = hold;
            hold = x1;
            x1 = x2;
            x2 = hold;
        }
    } else {
        if (x1 > x2) {
            hold = y1;
            y1 = y2;
            y2 = hold;
            hold = x1;
            x1 = x2;
            x2 = hold;
        }
    }

    delta_x = x2-x1;
    delta_y = y2-y1;

    p = dest->pixel_data + (x1 + y1*dest->width);

    if ( abs(delta_y) > abs(delta_x) ) {
        if (delta_x==0) {
            // decays to a VLine
            for (l=y1; l<=y2; l++) {
                *p = col;
                p += dest->width;
            }
        } else {
            // loop for every y
            delta_inc = ((delta_x) * 0x10000) / delta_y;
            for (l=y1; l<=y2; l++) {
				if (acc_inc < 0x7fff) {
					pel = col;
					pel.ch.a = fact[pel.ch.a][((0x7fff - acc_inc) * 0x7f)/0x7fff];
//					*(p-1) = pel;
					P_ALPHA((p-1), (&pel));
				}

				pel = col;
				pel.ch.a = fact[pel.ch.a][((0x7fff-abs(0x7fff-acc_inc)) * 0x7f)/0x7fff + 0x80];
				P_ALPHA((p), (&pel));
//				*p = pel;

				if (acc_inc > 0x8000) {
					pel = col;
					pel.ch.a = fact[pel.ch.a][((acc_inc - 0x7fff) * 0x7f)/0x7fff];
//					*(p+1) = pel;
					P_ALPHA((p+1), (&pel));
				}

                acc_inc += delta_inc;
                p += acc_inc >> 16;
                p += dest->width;
                acc_inc &= 0xffff;
            }
        }
    } else {
        if (delta_y==0) {
            // decays to an HLine
            for (l=x1; l<=x2; l++)
                *p++ = col;
        } else {
            // Loop for every x
            delta_inc = ((delta_y) * 0x10000) / delta_x;
            for (l=x1; l<=x2; l++) {
				if (acc_inc < 0x7fff) {
					pel = col;
					pel.ch.a = fact[pel.ch.a][((0x7fff - acc_inc) * 0x7f)/0x7fff];
//					*(p-dest->width) = pel;
					P_ALPHA((p-dest->width), (&pel));
				}

				pel = col;
				pel.ch.a = fact[pel.ch.a][((0x7fff-abs(0x7fff-acc_inc)) * 0x7f)/0x7fff + 0x80];
				P_ALPHA((p), (&pel));
//				*p = pel;

				if (acc_inc > 0x8000) {
					pel = col;
					pel.ch.a = fact[pel.ch.a][((acc_inc - 0x7fff) * 0x7f)/0x7fff];
//					*(p+dest->width) = pel;
					P_ALPHA((p+dest->width), (&pel));
				}

                acc_inc += delta_inc;
                p += (acc_inc >> 16) * dest->width;
                p++;
                acc_inc &= 0xffff;
            }
        }
    }
}


/* Danger, Will Robinson!  We do no clipping here, so trying to draw outside
   the buffer will merrily segfault! */
// Subpixel oversampled trishaded antialiased line routine
void line_aa_osa(double x1, double y1, double x2, double y2,
		_pel col, int osa, gimp_image_t *dest)
{
	int			l;
	double		hold;
	_pel		*p;
	int			delta_inc_x, acc_inc_x, delta_inc_y, acc_inc_y;
	double		delta_x, delta_y;
	_pel		pel;
	double		major;
	int			steps;

	if ( abs(y2-y1) > abs(x2-x1) ) {
		major = abs(y2-y1);
		if (y1 > y2) {
			hold = y1;
			y1 = y2;
			y2 = hold;
			hold = x1;
			x1 = x2;
			x2 = hold;
		}
	} else {
		major = abs(x2-x1);
		if (x1 > x2) {
			hold = y1;
			y1 = y2;
			y2 = hold;
			hold = x1;
			x1 = x2;
			x2 = hold;
		}
	}

	delta_x = x2-x1;
	delta_y = y2-y1;

	steps = (int)(major * osa);
	if (steps > major * osa)
		steps++;

	p = dest->pixel_data + ((int)x1 + (int)(y1)*dest->width);

	delta_inc_x = (int)(((delta_x) * 0x10000) / steps);
	delta_inc_y = (int)(((delta_y) * 0x10000) / steps);
	acc_inc_x = (int)(0x10000 * (x1-(int)(x1)));
	acc_inc_y = (int)(0x10000 * (y1-(int)(y1)));

	for (l=0; l<steps; l++) {
		if (acc_inc_x < 0x7fff) {
			pel = col;
//			pel.ch.a = fact[pel.ch.a][((0x7fff - acc_inc_x) * 0x7f)/(0x7fff * osa)];
			pel.ch.a = (int)(pel.ch.a * (
						(((0x7fff - acc_inc_x) + abs(0x7fff - acc_inc_y))* 0.5) /
						(double)(0x7fff * osa * 2)
			));
			P_ALPHA((p-1), (&pel));
		}

		if (acc_inc_y < 0x7fff) {
			pel = col;
//			pel.ch.a = fact[pel.ch.a][((0x7fff - acc_inc_y) * 0x7f)/(0x7fff * osa)];
			pel.ch.a = (int)(pel.ch.a * (
						(((0x7fff - acc_inc_y) + abs(0x7fff - acc_inc_x)) * 0.5) /
						(double)(0x7fff * osa * 2)
			));
			P_ALPHA((p-dest->width), (&pel));
		}

		pel = col;
//		pel.ch.a = fact[pel.ch.a][(((0x3fff-abs(0x3fff-acc_inc_y)) + (0x3fff-abs(0x3fff-acc_inc_x))) * 0x7f)/(0x7fff * osa) + 0x80];
		pel.ch.a = (int)(pel.ch.a * (
					(
					 (
					  (0x7fff-abs(0x7fff-acc_inc_y)) +
					  (0x7fff-abs(0x7fff-acc_inc_x))
					 ) * 0.75
					) /
					(double)(0x7fff * osa * 2) + 0.25
		));
		P_ALPHA((p), (&pel));

		if (acc_inc_x > 0x8000) {
			pel = col;
//			pel.ch.a = fact[pel.ch.a][((acc_inc_x - 0x7fff) * 0x7f)/(0x7fff * osa)];
			pel.ch.a = (int)(pel.ch.a * (
						(((acc_inc_x - 0x7fff) + abs(acc_inc_y - 0x7fff)) * 0.5) /
						(double)(0x7fff * osa * 2)
			));
			P_ALPHA((p+1), (&pel));
		}

		if (acc_inc_y > 0x8000) {
			pel = col;
//			pel.ch.a = fact[pel.ch.a][((acc_inc_y - 0x7fff) * 0x7f)/(0x7fff * osa)];
			pel.ch.a = (int)(pel.ch.a * (
						(((acc_inc_y - 0x7fff) + abs(acc_inc_x - 0x7fff)) * 0.5) /
						(double)(0x7fff * osa * 2)
			));
			P_ALPHA((p+dest->width), (&pel));
		}

		acc_inc_x += delta_inc_x;
		acc_inc_y += delta_inc_y;
		p += acc_inc_x >> 16;
		p += (acc_inc_y >> 16) * dest->width;
		acc_inc_y &= 0xffff;
		acc_inc_x &= 0xffff;
	}
}



// 4 point bezier curves:
// B(u) = p * (1 - u)³ + p  * 3 * u * (1 - u)² + p * 3 * u² * (1 - u) + p
//         0              1                       2                      3
void bezier(double x1, double y1, double cpx1, double cpy1,
		double cpx2, double cpy2, double x2, double y2, _pel colour,
		int osa, gimp_image_t *dest)
{
	double mum1, mum13, mu3;
	double px, py;

	double lacc;
	double dx, dy;
	double muacc;
	double mu;

	// Length estimation <<<
	lacc = 0;
	dx = abs(cpx1 - x1);
	dy = abs(cpy1 - y1);
	lacc += sqrt(dx*dx + dy*dy);
	dx = abs(cpx2 - cpx1);
	dy = abs(cpy2 - cpy1);
	lacc += sqrt(dx*dx + dy*dy);
	dx = abs(x2 - cpx2);
	dy = abs(y2 - cpy2);
	lacc += sqrt(dx*dx + dy*dy);
	// Length estimation >>>

//	lacc = bezier_arc_length(x1, y1, cpx1, cpy1, cpx2, cpx2, x2, y2);

//	fprintf(stderr, "bezier length: (%f)", lacc);
	muacc = 1.0 / lacc * osa;
	
	for (mu=0; mu<=1.0; mu+=muacc) {
		mum1 = 1 - mu;
		mum13 = mum1 * mum1 * mum1;
		mu3 = mu * mu * mu;

		px = mum13*x1 + 3*mu*mum1*mum1*cpx1 + 3*mu*mu*mum1*cpx2 + mu3*x2;
		py = mum13*y1 + 3*mu*mum1*mum1*cpy1 + 3*mu*mu*mum1*cpy2 + mu3*y2;

		quadshade(px, py, colour, osa, MD_ALPHA, dest);
	}
}


