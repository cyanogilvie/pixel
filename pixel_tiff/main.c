#ifdef DEBUG
#define DBG(format, args...) fprintf(stderr, "D: %s:%u:%s() " format, \
		                  __FILE__, __LINE__, __FUNCTION__ , ## args)
#else
#define DBG(format , args...)
#endif

#include <tiffio.h>
#include "2d.h"
#include "tcl_pmap.h"
#include <tclstuff.h>

gimp_image_t *loadtiff(char *tif_file) // {{{1
{
	uint32			i,l = 0;
	_pel			*d;
	_pel			*s;
	gimp_image_t	*dest = (gimp_image_t *)malloc(sizeof(gimp_image_t));
	uint16			planar;
	
	TIFF *tif = TIFFOpen( tif_file, "rb" );
	if (tif) {
		uint32	w, h;
		size_t	npixels;
		uint32	*raster;

		TIFFGetField( tif, TIFFTAG_IMAGEWIDTH, &w );
		TIFFGetField( tif, TIFFTAG_IMAGELENGTH, &h );
		TIFFGetField( tif, TIFFTAG_PLANARCONFIG, &planar );
		fprintf(stderr, "TIFFTAG_PLANARCONFIG: %d\n", planar);
		dest->width = w;
		dest->height = h;
		dest->bytes_per_pixel = 4;
		npixels = w * h;
		raster = (uint32 *)_TIFFmalloc(npixels * sizeof(uint32));
		if (raster != NULL) {
			if (TIFFReadRGBAImage( tif, w, h, raster, 0)) {
				dest->pixel_data = (_pel *)malloc(npixels * dest->bytes_per_pixel);
				d = dest->pixel_data;
				s = (_pel *)raster;
				d += h * w - w;
				for (i=0; i<npixels; i++) {
					d->ch.b = s->ch.r;
					d->ch.g = s->ch.g;
					d->ch.r = s->ch.b;
					d->ch.a = s->ch.a;
					d++; s++;
					if (++l == w) {
						d -= w << 1;
						l = 0;
					}
				}
			}
			_TIFFfree( raster );
		}
		TIFFClose( tif );
	}

	return dest;
}


int savetiff(char *filename, gimp_image_t *pmap) // {{{1
{
	TIFF	*tf;
	_pel	*s;
	int		i;
	
	tf = TIFFOpen(filename, "w");
	if (tf == NULL) return -1;

	TIFFSetField(tf, TIFFTAG_IMAGEWIDTH, pmap->width);
	TIFFSetField(tf, TIFFTAG_IMAGELENGTH, pmap->height);
	TIFFSetField(tf, TIFFTAG_PLANARCONFIG, 1);

	s = pmap->pixel_data;

	for (i=0; i<pmap->height; i++) {
		if (TIFFWriteScanline(tf, s, i, 0) == -1) {
			fprintf(stderr, "Oops: error writing TIFF\n");
			TIFFClose(tf);
			return -1;
		}
		s += pmap->width;
	}
	
	TIFFClose(tf);

	return 0;
}


// loadtiff filename {{{1
static int glue_loadtiff(ClientData foo, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[])
{
	CHECK_ARGS(1, "filename");

	Tcl_SetObjResult(interp, Tcl_NewPMAPObj(loadtiff(Tcl_GetString(objv[1]))));

	return TCL_OK;
}


// savetiff filename pmap {{{1
static int glue_savetiff(ClientData foo, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[])
{
	gimp_image_t	*pmap;
	int				err;
	
	CHECK_ARGS(2, "filename pmap");

	TEST_OK(Tcl_GetPMAPFromObj(interp, objv[2], &pmap));

	err = savetiff(Tcl_GetString(objv[1]), pmap);

	return TCL_OK;
}


// Init {{{1
int Pixel_tiff_Init(Tcl_Interp *interp)
{
	NEW_CMD("pixel::tiff::loadtiff", glue_loadtiff);
	NEW_CMD("pixel::tiff::savetiff", glue_savetiff);

	return TCL_OK;
}


