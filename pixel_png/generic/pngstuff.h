#ifndef _PIXEL_PNG_PNGSTUFF_H
#define _PIXEL_PNG_PNGSTUFF_H

extern int get_png_dimensions(char *filename, int *x, int *y);
extern gimp_image_t *read_png(char *filename);

#endif
