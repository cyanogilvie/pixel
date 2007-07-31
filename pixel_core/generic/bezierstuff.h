#ifndef _BEZIERSTUFF_H
#define _BEZIERSTUFF_H

#ifndef EXTERN_C
#  ifdef __cplusplus
#    define EXTERN_C extern "C"
#  else
#    define EXTERN_C extern
#  endif
#endif

EXTERN_C double bezier_arc_length(double x1, double y1, double cpx1, double cpy1,
		double cpx2, double cpy2, double x2, double y2);

#endif
