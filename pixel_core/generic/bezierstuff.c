#include "all.h"

// Code originally from Milton Friedman <uncle@stein.u.washington.edu> _____________
#define sqr(x) (x * x)

#define _ABS(x) (x < 0 ? -x : x)

static const double TOLERANCE = 0.0000001;  // Application specific tolerance

//extern double sqrt(double);

static double q1, q2, q3, q4, q5;      // These belong to balf()

typedef struct {
	double x, y;
} point2d;


//---------------------------------------------------------------------------
static double balf(double t)                   // Bezier Arc Length Function
{
	double result = q5 + t*(q4 + t*(q3 + t*(q2 + t*q1)));
	return sqrt(result);
}

//---------------------------------------------------------------------------
// NOTES:       TOLERANCE is a maximum error ratio
//                      if n_limit isn't a power of 2 it will be act like the next higher
//                      power of two.
static double Simpson (
		double (*f)(double),
		double a,
		double b,
		int n_limit,
		double TOLERANCE)
{
	int n = 1;
	double multiplier = (b - a)/6.0;
	double endsum = f(a) + f(b);
	double interval = (b - a)/2.0;
	double asum = 0;
	double bsum = f(a + interval);
	double est1 = multiplier * (endsum + 2 * asum + 4 * bsum);
	double est0 = 2 * est1;

	while(n < n_limit 
			&& (_ABS(est1) > 0 && _ABS((est1 - est0) / est1) > TOLERANCE)) {
		double interval_div_2n;
		int i;
		
		n *= 2;
		multiplier /= 2;
		interval /= 2;
		asum += bsum;
		bsum = 0;
		est0 = est1;
		interval_div_2n = interval / (2.0 * n);

		for (i = 1; i < 2 * n; i += 2) {
			double t = a + i * interval_div_2n;
			bsum += f(t);
		}

		est1 = multiplier*(endsum + 2*asum + 4*bsum);
	}

	return est1;
}

//
//---------------------------------------------------------------------------
//
double BezierArcLength(point2d p1, point2d p2, point2d p3, point2d p4)
{
	point2d k1, k2, k3, k4;

	/*
	k1 = -p1 + 3*(p2 - p3) + p4;
	k2 = 3*(p1 + p3) - 6*p2;
	k3 = 3*(p2 - p1);
	k4 = p1;
	*/

	k1.x = -p1.x + 3*(p2.x - p3.x) + p4.x;
	k1.y = -p1.y + 3*(p2.y - p3.y) + p4.y;
	k2.x = 3*(p1.x + p3.x) - 6*p2.x;
	k2.y = 3*(p1.y + p3.y) - 6*p2.y;
	k3.x = 3*(p2.x - p1.x);
	k3.y = 3*(p2.y - p1.y);
	k4.x = p1.x;
	k4.y = p1.y;


	q1 = 9.0*(sqr(k1.x) + sqr(k1.y));
	q2 = 12.0*(k1.x*k2.x + k1.y*k2.y);
	q3 = 3.0*(k1.x*k3.x + k1.y*k3.y) + 4.0*(sqr(k2.x) + sqr(k2.y));
	q4 = 4.0*(k2.x*k3.x + k2.y*k3.y);
	q5 = sqr(k3.x) + sqr(k3.y);

	return Simpson(balf, 0, 1, 1024, 0.001);
}

// Code originally from Milton Friedman <uncle@stein.u.washington.edu> ~~~~~~~~~~~~~~

double bezier_arc_length(double x1, double y1, double cpx1, double cpy1,
		double cpx2, double cpy2, double x2, double y2)
{
	point2d		p1, p2, p3, p4;

	p1.x = x1;
	p1.y = y1;

	p2.x = cpx1;
	p2.y = cpy1;
	
	p3.x = cpx2;
	p3.y = cpy2;
	
	p4.x = x2;
	p4.y = y2;

	return BezierArcLength(p1, p2, p3, p4);
}


