#include "all.h"

int get_png_dimensions(char *filename, int *x, int *y)
{
	png_structp		png_ptr;
	png_infop		info_ptr;
	png_uint_32		width, height;
	int				bit_depth, color_type, interlace_type;
	int				compression_type, filter_method;
	FILE			*fp;
	
	if ((fp = fopen(filename, "rb")) == NULL) return -1;

	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
			(png_voidp) NULL, NULL, NULL);
	if (png_ptr == NULL) {
		fclose(fp);
		return -1;
	}

	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL) {
		fclose(fp);
		png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
		return -1;
	}

	if (setjmp(png_ptr->jmpbuf)) {
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
		fclose(fp);
		return -1;
	}

	png_init_io(png_ptr, fp);
	png_read_info(png_ptr, info_ptr);
	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
			&interlace_type, &compression_type, &filter_method);

	*x = width;
	*y = height;
	
	png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
	fclose(fp);

	return 0;
}


gimp_image_t *read_png(char *filename)
{
	png_structp		png_ptr;
	png_infop		info_ptr;
	int				width, height;
	int				i;
	png_bytep		*row_pointers;
	FILE			*fp;
	gimp_image_t	*res;
	_pel			init;

	if (get_png_dimensions(filename, &width, &height) != 0)
		return NULL;

	if ((fp = fopen(filename, "rb")) == NULL) return NULL;

	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
			(png_voidp)NULL, NULL, NULL);
	if (png_ptr == NULL) {
		fclose(fp);
		return NULL;
	}

	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL) {
		fclose(fp);
		png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
		return NULL;
	}

	if (setjmp(png_ptr->jmpbuf)) {
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
		fclose(fp);
		return NULL;
	}

	png_init_io(png_ptr, fp);

	init.c = 0;
	res = pmap_new(width, height, init);

	pmap_clr(res, init);

	row_pointers = (png_bytep *)malloc(height * sizeof(png_bytep));
	for (i=0; i<height; i++) {
		row_pointers[i] = (png_bytep)(res->pixel_data + (i * width));
	}
	
	png_set_rows(png_ptr, info_ptr, row_pointers);
	
	/*
	png_read_png(png_ptr, info_ptr, 
			PNG_TRANSFORM_STRIP_16 |
			PNG_TRANSFORM_PACKING |
			PNG_TRANSFORM_EXPAND |
			PNG_TRANSFORM_SHIFT |
			PNG_TRANSFORM_SWAP_ALPHA, NULL);
			*/

	png_read_png(png_ptr, info_ptr, 
			PNG_TRANSFORM_STRIP_16 |
			PNG_TRANSFORM_PACKING |
			PNG_TRANSFORM_EXPAND, NULL);

	free(row_pointers);  row_pointers = NULL;

	png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
	fclose(fp);

	return res;
}


