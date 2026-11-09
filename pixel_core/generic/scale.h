#ifndef PIXEL_SCALE_H
#define PIXEL_SCALE_H

#include <pixelInt.h>

// Scale src (linear light) to new_w x new_h with centre-aligned separable
// passes of the windowed sinc of lanczos.c, wt input pixels each side (0:
// chosen for each dimension's ratio).
MODULE_SCOPE struct pmapf* pmapf_scale(const struct pmapf* src, int new_w, int new_h, int wt);

#endif
