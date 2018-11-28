#ifdef DEBUG
#define DBG(format, args...) fprintf(stderr, "D: %s:%u:%s() " format, \
		                  __FILE__, __LINE__, __FUNCTION__ , ## args)
#else
#define DBG(format , args...)
#endif

#include "all.h"

typedef union {
	uint8_t	chan[3];
	struct {
		uint8_t	b, g, r;
	} ch;
} rgb_pel;

typedef union {
	uint8_t	chan[2];
	struct {
		uint8_t	v, a;
	} ch;
} ga_pel;

static int glue_loadpng(cdata, interp, objc, objv) // loadpng filename {{{
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	gimp_image_t *	pmap;
	CHECK_ARGS(1, "filename");

	pmap = read_png(Tcl_GetString(objv[1]));

	if (pmap == NULL)
		THROW_ERROR("Error loading png file: ", Tcl_GetString(objv[1]));
	
	Tcl_SetObjResult(interp, Tcl_NewPMAPObj(pmap));

	return TCL_OK;
}

//}}}
/*
static int glue_savepng(cdata, interp, objc, objv) // savepng filename pmap {{{
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	int				err;
	gimp_image_t	*pmap;
	int				quality;
	
	CHECK_ARGS(2, "filename pmap");

	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[2], &pmap));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[3], &quality));


	if (err != 0)
		THROW_ERROR("Error saving png file: ", Tcl_GetString(objv[1]));
	
	return TCL_OK;
}

//}}}
*/
static int glue_is_png(cdata, interp, objc, objv) // is_png filename {{{
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	FILE			*fp;
	unsigned char	buf[8];

	CHECK_ARGS(1, "filename");

	fp = fopen(Tcl_GetString(objv[1]), "rb");
	if (fp == NULL)
		THROW_ERROR("Unable to open file: (", Tcl_GetString(objv[1]), ")");

	if (fread(buf, 1, 8, fp) != 8) {
		Tcl_SetObjResult(interp, Tcl_NewBooleanObj(0));
		return TCL_OK;
	}

	Tcl_SetObjResult(interp, Tcl_NewBooleanObj(!png_sig_cmp(buf, (png_size_t)0, 8)));

	return TCL_OK;
}

//}}}
static int glue_png_dimensions(cdata, interp, objc, objv) // png_dimensions filename {{{
	ClientData		cdata;
	Tcl_Interp		*interp;
	int				objc;
	Tcl_Obj *CONST	objv[];
{
	int			x, y;
	Tcl_Obj		*res;

	CHECK_ARGS(1, "filename");

	if (get_png_dimensions(Tcl_GetString(objv[1]), &x, &y) != 0)
		THROW_ERROR("Error reading PNG dimensions");

	res = Tcl_NewListObj(0, NULL);
	TEST_OK(Tcl_ListObjAppendElement(interp, res, Tcl_NewIntObj(x)));
	TEST_OK(Tcl_ListObjAppendElement(interp, res, Tcl_NewIntObj(y)));
	
	Tcl_SetObjResult(interp, res);

	return TCL_OK;
}

//}}}

void mem_write(png_structp png_ptr, png_bytep data, png_size_t length) //{{{
{
	struct write_buf*	write_buf = (struct write_buf*)png_get_io_ptr(png_ptr);

	if (!write_buf->data) {
		Tcl_IncrRefCount(write_buf->data = Tcl_NewByteArrayObj(data, length));
		write_buf->ofs = length;
	} else {
		int				buflen;
		unsigned char*	bytes;

		if (Tcl_IsShared(write_buf->data)) { // Shouldn't be possible
			Tcl_Obj* newdata = Tcl_DuplicateObj(write_buf->data);

			Tcl_DecrRefCount(write_buf->data);
			Tcl_IncrRefCount(write_buf->data = newdata);
		}

		bytes = Tcl_GetByteArrayFromObj(write_buf->data, &buflen);

		if (write_buf->ofs + length > buflen) { // Grow buffer
			int	newsize = buflen;

			while (write_buf->ofs + length > newsize) {
				if (newsize < 1048576) {
					// Start with a 1MB allocation
					newsize = 1048576;
				} else {
					if (newsize <= 5*1048576) {
						// Up to 5MB, double the buffer size when we run out
						newsize *= 2;
					} else {
						// Above that, add 5MB each time
						newsize += 5*1048576;
					}
				}
			}

			bytes = Tcl_SetByteArrayLength(write_buf->data, newsize);
		}

		memcpy(bytes + write_buf->ofs, data, length);
		write_buf->ofs += length;
		Tcl_InvalidateStringRep(write_buf->data);
	}
}

//}}}
void mem_flush(png_structp png_ptr) //{{{
{
	// NOP
}

//}}}
static int glue_encode(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]) //{{{
{
	png_structp			png_ptr = NULL;
	png_infop			info_ptr = NULL;
	png_color_8			sig_bit;
	int					compression = 3;
	struct write_buf	write_buf;
	gimp_image_t*		pmap = NULL;
	int					i;
	png_bytep*			row_pointers = NULL;

	write_buf.ofs = 0;
	write_buf.data = NULL;

	if (objc < 2 || objc > 3) {
		CHECK_ARGS(1, "pmap compression");
	}

	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[1], &pmap));

	if (objc >= 3) {
		TEST_OK(Tcl_GetIntFromObj(interp, objv[2], &compression));
	}

	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr) {
		Tcl_SetErrorCode(interp, "PIXEL", "PNG", "CREATE_WRITE_STRUCT", NULL);
		Tcl_SetObjResult(interp, Tcl_NewStringObj("Could not create PNG write struct", -1));
		goto error;
	}

	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr) {
		Tcl_SetErrorCode(interp, "PIXEL", "PNG", "CREATE_INFO_STRUCT", NULL);
		Tcl_SetObjResult(interp, Tcl_NewStringObj("Could not create PNG info struct", -1));
		goto error;
	}

	if (setjmp(png_jmpbuf(png_ptr))) {
		Tcl_SetErrorCode(interp, "PIXEL", "PNG", "ENCODE", NULL);
		Tcl_SetObjResult(interp, Tcl_NewStringObj("Error encoding PNG", -1));
		goto error;
	}

	png_set_write_fn(png_ptr, &write_buf, mem_write, mem_flush);

	png_set_IHDR(png_ptr, info_ptr, pmap->width, pmap->height, 8,
			PNG_COLOR_TYPE_RGB_ALPHA, 0,
			PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	png_set_bgr(png_ptr);

	sig_bit.red = 8;
	sig_bit.green = 8;
	sig_bit.blue = 8;
	sig_bit.alpha = 8;
	png_set_sBIT(png_ptr, info_ptr, &sig_bit);

	png_set_compression_level(png_ptr, compression);
	png_write_info(png_ptr, info_ptr);
	png_set_shift(png_ptr, &sig_bit);
	png_set_packing(png_ptr);

	row_pointers = (png_bytep*)malloc(pmap->height * sizeof(png_bytep));
	for (i=0; i<pmap->height; i++)
		row_pointers[i] = (png_bytep)(pmap->pixel_data + (i * pmap->width));

	png_write_image(png_ptr, row_pointers);

	png_write_end(png_ptr, info_ptr);

	free(row_pointers); row_pointers = NULL;
	png_destroy_write_struct(&png_ptr, &info_ptr); png_ptr = NULL; info_ptr = NULL;

	Tcl_SetByteArrayLength(write_buf.data, write_buf.ofs); // Trim the object to the actual length
	Tcl_InvalidateStringRep(write_buf.data);
	Tcl_SetObjResult(interp, write_buf.data);

	if (write_buf.data) {
		Tcl_DecrRefCount(write_buf.data); write_buf.data = NULL;
	}

	return TCL_OK;

error:
	free(row_pointers); row_pointers = NULL;
	png_destroy_write_struct(&png_ptr, &info_ptr); png_ptr = NULL; info_ptr = NULL;

	if (write_buf.data) {
		Tcl_DecrRefCount(write_buf.data); write_buf.data = NULL;
	}

	return TCL_ERROR;
}

//}}}
static void mem_read(png_structp png_ptr, png_bytep out, png_size_t count) //{{{
{
	struct png_membuf*	pngdata = (struct png_membuf*)png_get_io_ptr(png_ptr);

	if (pngdata->ofs + count > pngdata->len) {
		// TODO: how to signal an error from here?
		//memset((uint8_t*)out + (ptrdiff_t)count, 0, (pngdata->ofs+count) - pngdata->len);
		count = pngdata->len - pngdata->ofs;
	}

	memcpy((uint8_t*)out, (uint8_t*)(pngdata->buf)+(ptrdiff_t)pngdata->ofs, count);
	pngdata->ofs += count;
	//fprintf(stderr, "read %ld bytes, ofs: %d\n", count, pngdata->ofs);
}

//}}}
static int glue_decode(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]) //{{{
{
	png_structp			png_ptr;
	png_infop			info_ptr;
	png_uint_32			width, height;
	int					bitdepth=0, colourtype=-1;
	int					i, retval;
	png_bytep*			row_pointers = NULL;
	gimp_image_t*		pmap = NULL;
	_pel				init;
	struct png_membuf	pngdata;
	rgb_pel*			rgb_buf = NULL;
	ga_pel*				ga_buf = NULL;
	uint8_t*			gray_buf = NULL;

	CHECK_ARGS(1, "pngdata");

	pngdata.ofs = 0;
	pngdata.buf = Tcl_GetByteArrayFromObj(objv[1], &pngdata.len);

	if (pngdata.len < PNG_SIG_LEN || !png_check_sig(pngdata.buf, PNG_SIG_LEN)) {
		Tcl_SetErrorCode(interp, "PIXEL", "PNG", "NOT_A_PNG", NULL);
		THROW_ERROR("Not a PNG");
	}

	// First pass: find the width and height {{{
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (png_ptr == NULL) {
		Tcl_SetErrorCode(interp, "PIXEL", "PNG", "CREATE_READ_STRUCT", NULL);
		Tcl_SetObjResult(interp, Tcl_NewStringObj("Could not create PNG read struct", -1));
		goto error;
	}

	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL) {
		Tcl_SetErrorCode(interp, "PIXEL", "PNG", "CREATE_INFO_STRUCT", NULL);
		Tcl_SetObjResult(interp, Tcl_NewStringObj("Could not create PNG info struct", -1));
		goto error;
	}

	png_set_read_fn(png_ptr, &pngdata, mem_read);

	if (setjmp(png_ptr->jmpbuf)) {
		Tcl_SetErrorCode(interp, "PIXEL", "PNG", "READ", NULL);
		Tcl_SetObjResult(interp, Tcl_NewStringObj("Error reading PNG", -1));
		goto error;
	}

	png_read_info(png_ptr, info_ptr);

	//fprintf(stderr, "Reading IHDR\n");
	retval = png_get_IHDR(png_ptr, info_ptr, &width, &height, &bitdepth, &colourtype, NULL, NULL, NULL);
	if (retval != 1) {
		Tcl_SetErrorCode(interp, "PIXEL", "PNG", "METADATA", NULL);
		Tcl_SetObjResult(interp, Tcl_NewStringObj("Error reading PNG metadata", -1));
		goto error;
	}
	//fprintf(stderr, "width: %ld, height: %ld, bitdepth: %d, colourtype: %d, PNG_COLOR_TYPE_RGB_ALPHA: %d, PNG_COLOR_TYPE_RGB: %d\n", width, height, bitdepth, colourtype, PNG_COLOR_TYPE_RGB_ALPHA, PNG_COLOR_TYPE_RGB);
	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);  png_ptr = NULL; info_ptr = NULL;

	switch (bitdepth) {
		case 1:
		case 2:  /* Cases 2 and 4 aren't tested (can't find any test files) */
		case 4:  /* 1 bit is decoded to uint8_t samples though, so I'm assuming 2 and 4 are treated similarly */
			if (colourtype != PNG_COLOR_TYPE_GRAY) {
				Tcl_SetErrorCode(interp, "PIXEL", "PNG", "COLOURTYPE", NULL);
				Tcl_SetObjResult(interp, Tcl_ObjPrintf("Unsupported colour type: %d / bitdepth: %d combination", colourtype, bitdepth));
				goto error;
			}
			break;

		case 8:
		case 16:
			switch (colourtype) {
				case PNG_COLOR_TYPE_RGB_ALPHA: break;
				case PNG_COLOR_TYPE_RGB: break;
				case PNG_COLOR_TYPE_GA: break;
				case PNG_COLOR_TYPE_GRAY: break;
				case PNG_COLOR_TYPE_PALETTE: break;
				default:
					Tcl_SetErrorCode(interp, "PIXEL", "PNG", "COLOURTYPE", NULL);
					Tcl_SetObjResult(interp, Tcl_ObjPrintf("Unsupported colour type: %d / bitdepth: %d combination", colourtype, bitdepth));
					goto error;
			}
			break;

		default:
			Tcl_SetErrorCode(interp, "PIXEL", "PNG", "COLOURTYPE", NULL);
			Tcl_SetObjResult(interp, Tcl_ObjPrintf("Unsupported colour type: %d / bitdepth: %d combination", colourtype, bitdepth));
			goto error;
	}
	// First pass: find the width and height }}}

	// Have to re-init the context structs to use png_read_png for some reason {{{
	pngdata.ofs = 0;
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (png_ptr == NULL) {
		Tcl_SetErrorCode(interp, "PIXEL", "PNG", "CREATE_READ_STRUCT", NULL);
		Tcl_SetObjResult(interp, Tcl_NewStringObj("Could not create PNG read struct", -1));
		goto error;
	}

	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL) {
		Tcl_SetErrorCode(interp, "PIXEL", "PNG", "CREATE_INFO_STRUCT", NULL);
		Tcl_SetObjResult(interp, Tcl_NewStringObj("Could not create PNG info struct", -1));
		goto error;
	}

	png_set_read_fn(png_ptr, &pngdata, mem_read);

	if (setjmp(png_ptr->jmpbuf)) {
		Tcl_SetErrorCode(interp, "PIXEL", "PNG", "READ", NULL);
		Tcl_SetObjResult(interp, Tcl_NewStringObj("Error reading PNG", -1));
		goto error;
	}
	// Have to re-init the context structs to use png_read_png for some reason }}}

	init.c = 0;
	pmap = pmap_new(width, height, init);

	pmap_clr(pmap, init);

	row_pointers = (png_bytep*)malloc(height * sizeof(png_bytep));

	switch (colourtype) {
		case PNG_COLOR_TYPE_RGB:
			rgb_buf = (rgb_pel*)malloc((sizeof(rgb_pel)+1) * width * height);
			for (i=0; i<height; i++)
				row_pointers[i] = (png_bytep)(rgb_buf + (i * width));
			break;

		case PNG_COLOR_TYPE_GA:
			ga_buf = (ga_pel*)malloc(sizeof(ga_pel) * width * height);
			for (i=0; i<height; i++)
				row_pointers[i] = (png_bytep)(ga_buf + (i * width));
			break;

		case PNG_COLOR_TYPE_GRAY:
			gray_buf = (uint8_t*)malloc(width*height);
			for (i=0; i<height; i++)
				row_pointers[i] = (png_bytep)(gray_buf + (i * width));
			break;

		default:
			for (i=0; i<height; i++)
				row_pointers[i] = (png_bytep)(pmap->pixel_data + (i * width));
	}

	png_set_rows(png_ptr, info_ptr, row_pointers);

	png_read_png(png_ptr, info_ptr,
			PNG_TRANSFORM_STRIP_16 |
			PNG_TRANSFORM_PACKING |
			PNG_TRANSFORM_SHIFT |
			PNG_TRANSFORM_BGR |
			PNG_TRANSFORM_EXPAND, NULL);

	switch (colourtype) {
		case PNG_COLOR_TYPE_RGB: { //{{{
				rgb_pel*	s = rgb_buf;
				_pel*		d = pmap->pixel_data;
				int			i = width * height, c;

				while (i--) {
					for (c=0; c<3; c++)
						d->chan[c] = s->chan[c];

					d->ch.a = 0xff;
					d++;
					s++;
				}
			}
			break;
			//}}}
		case PNG_COLOR_TYPE_GA: { //{{{
				ga_pel*		s = ga_buf;
				_pel*		d = pmap->pixel_data;
				int			i = width * height, c;

				while (i--) {
					for (c=0; c<3; c++)
						d->chan[c] = s->chan[0];

					d->ch.a = s->ch.a;
					d++;
					s++;
				}
			}
			break;
			//}}}
		case PNG_COLOR_TYPE_GRAY: { //{{{
				uint8_t*	s = gray_buf;
				_pel*		d = pmap->pixel_data;
				int			i = width * height, c;

				while (i--) {
					for (c=0; c<3; c++)
						d->chan[c] = *s;

					d->ch.a = 0xff;
					d++;
					s++;
				}
			}
			break;
			//}}}
	}

	free(row_pointers);  row_pointers = NULL;

	if (rgb_buf)    { free(rgb_buf);    rgb_buf = NULL;    }
	if (ga_buf)     { free(ga_buf);     ga_buf = NULL;     }
	if (gray_buf)   { free(gray_buf);   gray_buf = NULL;   }

	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);  png_ptr = NULL; info_ptr = NULL;
	Tcl_SetObjResult(interp, Tcl_NewPMAPObj(pmap));
	return TCL_OK;

error:
	if (row_pointers) { free(row_pointers);  row_pointers = NULL; }
	if (pmap)         { free(pmap);          pmap = NULL;         }
	if (rgb_buf)      { free(rgb_buf);       rgb_buf = NULL;      }
	if (ga_buf)       { free(ga_buf);        ga_buf = NULL;       }
	if (gray_buf)     { free(gray_buf);      gray_buf = NULL;     }

	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);  png_ptr = NULL; info_ptr = NULL;
	return TCL_ERROR;
}

//}}}
int Pixel_png_Init(Tcl_Interp *interp) // Init {{{
{
	if (Tcl_InitStubs(interp, "8.4", 0) == NULL) return TCL_ERROR;
	if (Pixel_InitStubs(interp, "3.3", 0) == NULL) return TCL_ERROR;

	NEW_CMD("pixel::png::loadpng", glue_loadpng);
	//NEW_CMD("pixel::png::savepng", glue_savepng);
	NEW_CMD("pixel::png::is_png", glue_is_png);
	NEW_CMD("pixel::png::png_dimensions", glue_png_dimensions);
	NEW_CMD("pixel::png::encode", glue_encode);
	NEW_CMD("pixel::png::decode", glue_decode);

	if (Tcl_PkgProvide(interp, PACKAGE_NAME, PACKAGE_VERSION) != TCL_OK)
		return TCL_ERROR;

	return TCL_OK;
}

//}}}
