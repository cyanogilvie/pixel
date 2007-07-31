#ifndef _SHADERS_H
#define _SHADERS_H

#ifndef EXTERN_C
#  ifdef __cplusplus
#    define EXTERN_C extern "C"
#  else
#    define EXTERN_C extern
#  endif
#endif

EXTERN_C void quadshade(double x, double y, _pel colour, int osa, int flags, gimp_image_t *dest);

#endif
