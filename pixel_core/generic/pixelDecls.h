

#ifndef EXTERN
#  ifdef __cplusplus
#    define EXTERN extern "C"
#  else
#    define EXTERN extern
#  endif
#endif

#undef TCL_STORAGE_CLASS
#define TCL_STORAGE_CLASS DLLEXPORT

/* !BEGIN!: Do not edit below this line. */

/*
 * Exported function declarations:
 */

#ifndef pmap_new_TCL_DECLARED
#define pmap_new_TCL_DECLARED
/* 0 */
EXTERN gimp_image_t *	pmap_new (int x, int y, _pel colour);
#endif
#ifndef pmap_dup_TCL_DECLARED
#define pmap_dup_TCL_DECLARED
/* 1 */
EXTERN gimp_image_t *	pmap_dup (gimp_image_t * src);
#endif
#ifndef row_blend_TCL_DECLARED
#define row_blend_TCL_DECLARED
/* 2 */
EXTERN void		row_blend (_pel * d, _pel * s, int len, int flags);
#endif
#ifndef row_set_TCL_DECLARED
#define row_set_TCL_DECLARED
/* 3 */
EXTERN void		row_set (_pel * dest, _pel colour, int len, 
				int flags);
#endif
#ifndef pmap_clr_TCL_DECLARED
#define pmap_clr_TCL_DECLARED
/* 4 */
EXTERN void		pmap_clr (gimp_image_t * dest, _pel colour);
#endif
#ifndef pmap_cut_TCL_DECLARED
#define pmap_cut_TCL_DECLARED
/* 5 */
EXTERN gimp_image_t *	pmap_cut (gimp_image_t * src, int x1, int y1, int x2, 
				int y2);
#endif
#ifndef pmap_paste_TCL_DECLARED
#define pmap_paste_TCL_DECLARED
/* 6 */
EXTERN void		pmap_paste (gimp_image_t * dest, gimp_image_t * src, 
				int xofs, int yofs, int flags);
#endif
#ifndef pmap_paste_ref_TCL_DECLARED
#define pmap_paste_ref_TCL_DECLARED
/* 7 */
EXTERN void		pmap_paste_ref (gimp_image_t * dest, 
				gimp_image_t * src, gimp_image_t * ref, 
				int xofs, int yofs, int flags);
#endif
#ifndef pmap_patch_TCL_DECLARED
#define pmap_patch_TCL_DECLARED
/* 8 */
EXTERN void		pmap_patch (gimp_image_t * dest, gimp_image_t * src, 
				int sx, int sy, int sw, int sh, int dx, 
				int dy, int flags);
#endif
#ifndef pmap_compose_TCL_DECLARED
#define pmap_compose_TCL_DECLARED
/* 9 */
EXTERN gimp_image_t *	pmap_compose (gimp_image_t * pmap1, 
				gimp_image_t * pmap2, int xofs, int yofs, 
				int flags);
#endif
#ifndef pmap_free_TCL_DECLARED
#define pmap_free_TCL_DECLARED
/* 10 */
EXTERN void		pmap_free (gimp_image_t ** pmap);
#endif
#ifndef pmap_filter_TCL_DECLARED
#define pmap_filter_TCL_DECLARED
/* 11 */
EXTERN void		pmap_filter (gimp_image_t * dest, int flags, 
				double factor);
#endif
#ifndef pmap_dropshadow_TCL_DECLARED
#define pmap_dropshadow_TCL_DECLARED
/* 12 */
EXTERN gimp_image_t *	pmap_dropshadow (gimp_image_t * src, int blur);
#endif
#ifndef pmap_rotate_TCL_DECLARED
#define pmap_rotate_TCL_DECLARED
/* 13 */
EXTERN gimp_image_t *	pmap_rotate (gimp_image_t * src, int quads);
#endif
#ifndef digest_region_TCL_DECLARED
#define digest_region_TCL_DECLARED
/* 14 */
EXTERN void		digest_region (gimp_image_t * src, int x, int y, 
				int w, int h, unsigned int * r, 
				unsigned int * g, unsigned int * b, 
				unsigned int * a);
#endif
#ifndef put_pixel_TCL_DECLARED
#define put_pixel_TCL_DECLARED
/* 15 */
EXTERN void		put_pixel (gimp_image_t * dest, int x, int y, 
				_pel col, int flags);
#endif
#ifndef get_pixel_TCL_DECLARED
#define get_pixel_TCL_DECLARED
/* 16 */
EXTERN _pel		get_pixel (gimp_image_t * src, int x, int y);
#endif
#ifndef rgb2hsv_TCL_DECLARED
#define rgb2hsv_TCL_DECLARED
/* 17 */
EXTERN void		rgb2hsv (unsigned char r, unsigned char g, 
				unsigned char b, double * h, double * s, 
				double * v);
#endif
#ifndef hsv2rgb_TCL_DECLARED
#define hsv2rgb_TCL_DECLARED
/* 18 */
EXTERN void		hsv2rgb (double h, double s, double v, 
				unsigned char * r, unsigned char * g, 
				unsigned char * b);
#endif
#ifndef do_dirty_tricks_TCL_DECLARED
#define do_dirty_tricks_TCL_DECLARED
/* 19 */
EXTERN void		do_dirty_tricks (void);
#endif
#ifndef init_2d_TCL_DECLARED
#define init_2d_TCL_DECLARED
/* 20 */
EXTERN void		init_2d (void);
#endif
#ifndef scale_pmap_TCL_DECLARED
#define scale_pmap_TCL_DECLARED
/* 21 */
EXTERN gimp_image_t *	scale_pmap (gimp_image_t * src, int ssx, int ssy, 
				int ssw, int ssh, int ddx, int ddy, int ddw, 
				int ddh);
#endif
#ifndef bezier_arc_length_TCL_DECLARED
#define bezier_arc_length_TCL_DECLARED
/* 22 */
EXTERN double		bezier_arc_length (double x1, double y1, double cpx1, 
				double cpy1, double cpx2, double cpy2, 
				double x2, double y2);
#endif
#ifndef asm_pelset_TCL_DECLARED
#define asm_pelset_TCL_DECLARED
/* 23 */
EXTERN void		asm_pelset (_pel * dest, _pel rgba, uint32 count);
#endif
/* Slot 24 is reserved */
#ifndef box_TCL_DECLARED
#define box_TCL_DECLARED
/* 25 */
EXTERN void		box (gimp_image_t * dest, int x, int y, int w, int h, 
				_pel colour, int flags);
#endif
#ifndef line_TCL_DECLARED
#define line_TCL_DECLARED
/* 26 */
EXTERN void		line (int x1, int y1, int x2, int y2, _pel col, 
				gimp_image_t * dest);
#endif
#ifndef line_aa_TCL_DECLARED
#define line_aa_TCL_DECLARED
/* 27 */
EXTERN void		line_aa (int x1, int y1, int x2, int y2, _pel col, 
				gimp_image_t * dest);
#endif
#ifndef line_aa_osa_TCL_DECLARED
#define line_aa_osa_TCL_DECLARED
/* 28 */
EXTERN void		line_aa_osa (double x1, double y1, double x2, 
				double y2, _pel col, int osa, 
				gimp_image_t * dest);
#endif
#ifndef bezier_TCL_DECLARED
#define bezier_TCL_DECLARED
/* 29 */
EXTERN void		bezier (double x1, double y1, double cpx1, 
				double cpy1, double cpx2, double cpy2, 
				double x2, double y2, _pel colour, int osa, 
				gimp_image_t * dest);
#endif
#ifndef quadshade_TCL_DECLARED
#define quadshade_TCL_DECLARED
/* 30 */
EXTERN void		quadshade (double x, double y, _pel colour, int osa, 
				int flags, gimp_image_t * dest);
#endif
#ifndef Tcl_SetPMAPObj_TCL_DECLARED
#define Tcl_SetPMAPObj_TCL_DECLARED
/* 31 */
EXTERN void		Tcl_SetPMAPObj (Tcl_Obj * obj, gimp_image_t * pmap);
#endif
#ifndef Tcl_NewPMAPObj_TCL_DECLARED
#define Tcl_NewPMAPObj_TCL_DECLARED
/* 32 */
EXTERN Tcl_Obj *	Tcl_NewPMAPObj (gimp_image_t * pmap);
#endif
#ifndef Tcl_GetPMAPFromObj_TCL_DECLARED
#define Tcl_GetPMAPFromObj_TCL_DECLARED
/* 33 */
EXTERN int		Tcl_GetPMAPFromObj (Tcl_Interp * interp, 
				Tcl_Obj * obj, gimp_image_t ** pmap);
#endif
#ifndef read_vfs_file_TCL_DECLARED
#define read_vfs_file_TCL_DECLARED
/* 34 */
EXTERN const unsigned char * read_vfs_file (const char * filename, int * len, 
				int binary);
#endif
#ifndef read_vfs_file2_TCL_DECLARED
#define read_vfs_file2_TCL_DECLARED
/* 35 */
EXTERN int		read_vfs_file2 (Tcl_Interp * interp, 
				const char * filename, unsigned char ** buf, 
				int * len, int binary);
#endif
#ifndef rle_decode_TCL_DECLARED
#define rle_decode_TCL_DECLARED
/* 36 */
EXTERN gimp_image_t *	rle_decode (const unsigned char * rle_data, 
				unsigned int data_len, int * status);
#endif
#ifndef rle_encode_TCL_DECLARED
#define rle_encode_TCL_DECLARED
/* 37 */
EXTERN unsigned char *	rle_encode (const gimp_image_t * pmap, 
				unsigned int * data_len, int * status);
#endif

typedef struct PixelStubs {
    int magic;
    struct PixelStubHooks *hooks;

    gimp_image_t * (*pmap_new) (int x, int y, _pel colour); /* 0 */
    gimp_image_t * (*pmap_dup) (gimp_image_t * src); /* 1 */
    void (*row_blend) (_pel * d, _pel * s, int len, int flags); /* 2 */
    void (*row_set) (_pel * dest, _pel colour, int len, int flags); /* 3 */
    void (*pmap_clr) (gimp_image_t * dest, _pel colour); /* 4 */
    gimp_image_t * (*pmap_cut) (gimp_image_t * src, int x1, int y1, int x2, int y2); /* 5 */
    void (*pmap_paste) (gimp_image_t * dest, gimp_image_t * src, int xofs, int yofs, int flags); /* 6 */
    void (*pmap_paste_ref) (gimp_image_t * dest, gimp_image_t * src, gimp_image_t * ref, int xofs, int yofs, int flags); /* 7 */
    void (*pmap_patch) (gimp_image_t * dest, gimp_image_t * src, int sx, int sy, int sw, int sh, int dx, int dy, int flags); /* 8 */
    gimp_image_t * (*pmap_compose) (gimp_image_t * pmap1, gimp_image_t * pmap2, int xofs, int yofs, int flags); /* 9 */
    void (*pmap_free) (gimp_image_t ** pmap); /* 10 */
    void (*pmap_filter) (gimp_image_t * dest, int flags, double factor); /* 11 */
    gimp_image_t * (*pmap_dropshadow) (gimp_image_t * src, int blur); /* 12 */
    gimp_image_t * (*pmap_rotate) (gimp_image_t * src, int quads); /* 13 */
    void (*digest_region) (gimp_image_t * src, int x, int y, int w, int h, unsigned int * r, unsigned int * g, unsigned int * b, unsigned int * a); /* 14 */
    void (*put_pixel) (gimp_image_t * dest, int x, int y, _pel col, int flags); /* 15 */
    _pel (*get_pixel) (gimp_image_t * src, int x, int y); /* 16 */
    void (*rgb2hsv) (unsigned char r, unsigned char g, unsigned char b, double * h, double * s, double * v); /* 17 */
    void (*hsv2rgb) (double h, double s, double v, unsigned char * r, unsigned char * g, unsigned char * b); /* 18 */
    void (*do_dirty_tricks) (void); /* 19 */
    void (*init_2d) (void); /* 20 */
    gimp_image_t * (*scale_pmap) (gimp_image_t * src, int ssx, int ssy, int ssw, int ssh, int ddx, int ddy, int ddw, int ddh); /* 21 */
    double (*bezier_arc_length) (double x1, double y1, double cpx1, double cpy1, double cpx2, double cpy2, double x2, double y2); /* 22 */
    void (*asm_pelset) (_pel * dest, _pel rgba, uint32 count); /* 23 */
    void *reserved24;
    void (*box) (gimp_image_t * dest, int x, int y, int w, int h, _pel colour, int flags); /* 25 */
    void (*line) (int x1, int y1, int x2, int y2, _pel col, gimp_image_t * dest); /* 26 */
    void (*line_aa) (int x1, int y1, int x2, int y2, _pel col, gimp_image_t * dest); /* 27 */
    void (*line_aa_osa) (double x1, double y1, double x2, double y2, _pel col, int osa, gimp_image_t * dest); /* 28 */
    void (*bezier) (double x1, double y1, double cpx1, double cpy1, double cpx2, double cpy2, double x2, double y2, _pel colour, int osa, gimp_image_t * dest); /* 29 */
    void (*quadshade) (double x, double y, _pel colour, int osa, int flags, gimp_image_t * dest); /* 30 */
    void (*tcl_SetPMAPObj) (Tcl_Obj * obj, gimp_image_t * pmap); /* 31 */
    Tcl_Obj * (*tcl_NewPMAPObj) (gimp_image_t * pmap); /* 32 */
    int (*tcl_GetPMAPFromObj) (Tcl_Interp * interp, Tcl_Obj * obj, gimp_image_t ** pmap); /* 33 */
    const unsigned char * (*read_vfs_file) (const char * filename, int * len, int binary); /* 34 */
    int (*read_vfs_file2) (Tcl_Interp * interp, const char * filename, unsigned char ** buf, int * len, int binary); /* 35 */
    gimp_image_t * (*rle_decode) (const unsigned char * rle_data, unsigned int data_len, int * status); /* 36 */
    unsigned char * (*rle_encode) (const gimp_image_t * pmap, unsigned int * data_len, int * status); /* 37 */
} PixelStubs;

#ifdef __cplusplus
extern "C" {
#endif
extern PixelStubs *pixelStubsPtr;
#ifdef __cplusplus
}
#endif

#if defined(USE_PIXEL_STUBS) && !defined(USE_PIXEL_STUB_PROCS)

/*
 * Inline function declarations:
 */

#ifndef pmap_new
#define pmap_new \
	(pixelStubsPtr->pmap_new) /* 0 */
#endif
#ifndef pmap_dup
#define pmap_dup \
	(pixelStubsPtr->pmap_dup) /* 1 */
#endif
#ifndef row_blend
#define row_blend \
	(pixelStubsPtr->row_blend) /* 2 */
#endif
#ifndef row_set
#define row_set \
	(pixelStubsPtr->row_set) /* 3 */
#endif
#ifndef pmap_clr
#define pmap_clr \
	(pixelStubsPtr->pmap_clr) /* 4 */
#endif
#ifndef pmap_cut
#define pmap_cut \
	(pixelStubsPtr->pmap_cut) /* 5 */
#endif
#ifndef pmap_paste
#define pmap_paste \
	(pixelStubsPtr->pmap_paste) /* 6 */
#endif
#ifndef pmap_paste_ref
#define pmap_paste_ref \
	(pixelStubsPtr->pmap_paste_ref) /* 7 */
#endif
#ifndef pmap_patch
#define pmap_patch \
	(pixelStubsPtr->pmap_patch) /* 8 */
#endif
#ifndef pmap_compose
#define pmap_compose \
	(pixelStubsPtr->pmap_compose) /* 9 */
#endif
#ifndef pmap_free
#define pmap_free \
	(pixelStubsPtr->pmap_free) /* 10 */
#endif
#ifndef pmap_filter
#define pmap_filter \
	(pixelStubsPtr->pmap_filter) /* 11 */
#endif
#ifndef pmap_dropshadow
#define pmap_dropshadow \
	(pixelStubsPtr->pmap_dropshadow) /* 12 */
#endif
#ifndef pmap_rotate
#define pmap_rotate \
	(pixelStubsPtr->pmap_rotate) /* 13 */
#endif
#ifndef digest_region
#define digest_region \
	(pixelStubsPtr->digest_region) /* 14 */
#endif
#ifndef put_pixel
#define put_pixel \
	(pixelStubsPtr->put_pixel) /* 15 */
#endif
#ifndef get_pixel
#define get_pixel \
	(pixelStubsPtr->get_pixel) /* 16 */
#endif
#ifndef rgb2hsv
#define rgb2hsv \
	(pixelStubsPtr->rgb2hsv) /* 17 */
#endif
#ifndef hsv2rgb
#define hsv2rgb \
	(pixelStubsPtr->hsv2rgb) /* 18 */
#endif
#ifndef do_dirty_tricks
#define do_dirty_tricks \
	(pixelStubsPtr->do_dirty_tricks) /* 19 */
#endif
#ifndef init_2d
#define init_2d \
	(pixelStubsPtr->init_2d) /* 20 */
#endif
#ifndef scale_pmap
#define scale_pmap \
	(pixelStubsPtr->scale_pmap) /* 21 */
#endif
#ifndef bezier_arc_length
#define bezier_arc_length \
	(pixelStubsPtr->bezier_arc_length) /* 22 */
#endif
#ifndef asm_pelset
#define asm_pelset \
	(pixelStubsPtr->asm_pelset) /* 23 */
#endif
/* Slot 24 is reserved */
#ifndef box
#define box \
	(pixelStubsPtr->box) /* 25 */
#endif
#ifndef line
#define line \
	(pixelStubsPtr->line) /* 26 */
#endif
#ifndef line_aa
#define line_aa \
	(pixelStubsPtr->line_aa) /* 27 */
#endif
#ifndef line_aa_osa
#define line_aa_osa \
	(pixelStubsPtr->line_aa_osa) /* 28 */
#endif
#ifndef bezier
#define bezier \
	(pixelStubsPtr->bezier) /* 29 */
#endif
#ifndef quadshade
#define quadshade \
	(pixelStubsPtr->quadshade) /* 30 */
#endif
#ifndef Tcl_SetPMAPObj
#define Tcl_SetPMAPObj \
	(pixelStubsPtr->tcl_SetPMAPObj) /* 31 */
#endif
#ifndef Tcl_NewPMAPObj
#define Tcl_NewPMAPObj \
	(pixelStubsPtr->tcl_NewPMAPObj) /* 32 */
#endif
#ifndef Tcl_GetPMAPFromObj
#define Tcl_GetPMAPFromObj \
	(pixelStubsPtr->tcl_GetPMAPFromObj) /* 33 */
#endif
#ifndef read_vfs_file
#define read_vfs_file \
	(pixelStubsPtr->read_vfs_file) /* 34 */
#endif
#ifndef read_vfs_file2
#define read_vfs_file2 \
	(pixelStubsPtr->read_vfs_file2) /* 35 */
#endif
#ifndef rle_decode
#define rle_decode \
	(pixelStubsPtr->rle_decode) /* 36 */
#endif
#ifndef rle_encode
#define rle_encode \
	(pixelStubsPtr->rle_encode) /* 37 */
#endif

#endif /* defined(USE_PIXEL_STUBS) && !defined(USE_PIXEL_STUB_PROCS) */

/* !END!: Do not edit above this line. */

/*
#undef TCL_STORAGE_CLASS
#define TCL_STORAGE_CLASS DLLIMPORT
*/
