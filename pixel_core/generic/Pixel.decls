library Pixel
interface pixel
declare 0 generic {
	gimp_image_t *pmap_new(int x, int y, _pel colour)
}
declare 1 generic {
	gimp_image_t *pmap_dup(gimp_image_t *src)
}
declare 2 generic {
	void row_blend(_pel *d, _pel *s, int len, int flags)
}
declare 3 generic {
	void row_set(_pel *dest, _pel colour, int len, int flags)
}
declare 4 generic {
	void pmap_clr(gimp_image_t *dest, _pel colour)
}
declare 5 generic {
	gimp_image_t *pmap_cut(gimp_image_t *src, int x1, int y1, int x2, int y2)
}
declare 6 generic {
	void pmap_paste(gimp_image_t *dest, gimp_image_t *src, int xofs, int yofs, int flags)
}
declare 7 generic {
	void pmap_paste_ref(gimp_image_t *dest, gimp_image_t *src, gimp_image_t *ref, int xofs, int yofs, int flags)
}
declare 8 generic {
	void pmap_patch(gimp_image_t *dest, gimp_image_t *src, int sx, int sy, int sw, int sh, int dx, int dy, int flags)
}
declare 9 generic {
	gimp_image_t *pmap_compose(gimp_image_t *pmap1, gimp_image_t *pmap2, int xofs, int yofs, int flags)
}
declare 10 generic {
	void pmap_free(gimp_image_t **pmap)
}
declare 11 generic {
	void pmap_filter(gimp_image_t *dest, int flags, double factor)
}
declare 12 generic {
	gimp_image_t *pmap_dropshadow (gimp_image_t *src, int blur)
}
declare 13 generic {
	gimp_image_t *pmap_rotate(gimp_image_t *src, int quads)
}
declare 14 generic {
	void digest_region(gimp_image_t *src, int x, int y, int w, int h, unsigned int *r, unsigned int *g, unsigned int *b, unsigned int *a)
}
declare 15 generic {
	void put_pixel(gimp_image_t *dest, int x, int y, _pel col, int flags)
}
declare 16 generic {
	_pel get_pixel(gimp_image_t *src, int x, int y)
}
declare 17 generic {
	void rgb2hsv(unsigned char r, unsigned char g, unsigned char b, double *h,  double *s,  double *v)
}
declare 18 generic {
	void hsv2rgb(double h,  double s,  double v, unsigned char *r, unsigned char *g, unsigned char *b)
}
declare 19 generic {
	void do_dirty_tricks(void)
}
declare 20 generic {
	void init_2d(void)
}
declare 21 generic {
	gimp_image_t *scale_pmap(gimp_image_t *src, int ssx, int ssy, int ssw, int ssh, int ddx, int ddy, int ddw, int ddh)
}


declare 22 generic {
	double bezier_arc_length(double x1, double y1, double cpx1, double cpy1, double cpx2, double cpy2, double x2, double y2)
}
declare 23 generic {
	void asm_pelset(_pel *dest, _pel rgba, uint32 count)
}
declare 25 generic {
	void box(gimp_image_t *dest, int x, int y, int w, int h, _pel colour, int flags)
}
declare 26 generic {
	void line(int x1, int y1, int x2, int y2, _pel col, gimp_image_t *dest)
}
declare 27 generic {
	void line_aa(int x1, int y1, int x2, int y2, _pel col, gimp_image_t *dest)
}
declare 28 generic {
	void line_aa_osa(double x1, double y1, double x2, double y2, _pel col, int osa, gimp_image_t *dest)
}
declare 29 generic {
	void bezier(double x1, double y1, double cpx1, double cpy1, double cpx2, double cpy2, double x2, double y2, _pel colour, int osa, gimp_image_t *dest)
}
declare 30 generic {
	void quadshade(double x, double y, _pel colour, int osa, int flags, gimp_image_t *dest)
}
declare 31 generic {
	void Tcl_SetPMAPObj(Tcl_Obj * obj, gimp_image_t * pmap)
}
declare 32 generic {
	Tcl_Obj *Tcl_NewPMAPObj(gimp_image_t * pmap)
}
declare 33 generic {
	int Tcl_GetPMAPFromObj(Tcl_Interp * interp, Tcl_Obj * obj, gimp_image_t ** pmap)
}
declare 34 generic {
	const unsigned char *read_vfs_file(const char *filename, int *len, int binary)
}
declare 35 generic {
	int read_vfs_file2(Tcl_Interp *interp, const char *filename, unsigned char **buf, int *len, int binary)
}
declare 36 generic {
	gimp_image_t *rle_decode(const unsigned char *rle_data, unsigned int data_len, int *status)
}
declare 37 generic {
	unsigned char *rle_encode(const gimp_image_t *pmap, unsigned int *data_len, int *status)
}
declare 38 generic {
	int Tcl_IsPMAPObj(Tcl_Obj* obj)
}
declare 39 generic {
	struct pmapf* pmapf_new(int width, int height)
}
declare 40 generic {
	Tcl_Obj* Pixel_NewPMAPFObj(struct pmapf* pmapf)
}
declare 41 generic {
	int Pixel_GetPMAPFFromObj(Tcl_Interp* interp, Tcl_Obj* obj, struct pmapf** pmapf)
}
declare 42 generic {
	void pmapf_free(struct pmapf** pmapf)
}
declare 43 generic {
	struct pmapf* pmap_to_pmapf(gimp_image_t* pmap)
}
declare 44 generic {
	gimp_image_t* pmapf_to_pmap(struct pmapf* pmapf)
}
declare 45 generic {
	int Pixel_GetPELFFromObj(Tcl_Interp* interp, Tcl_Obj* obj, pelf* out)
}
declare 46 generic {
	struct pmapf* pmapf_gradient_radial(int width, int height, pelf* centre_colour, pelf* outer_colour)
}
declare 47 generic {
	struct pmapf* pmapf_gradient_linear_v(int width, int height, pelf* top_colour, pelf* bottom_colour)
}
declare 48 generic {
	struct pmapf* pmapf_alpha_over(struct pmapf* dest, struct pmapf* src, int xofs, int yofs)
}
declare 49 generic {
	void pmapf_clr(struct pmapf* dest, pelf colour)
}
