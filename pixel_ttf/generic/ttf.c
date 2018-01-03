//#include <Pixel/pixel.h>
extern double *pmap_new_x(int x, int y);
#include <pixel.h>
#include "ttf.h"
#include <ft2build.h>
#include <string.h>
#include FT_FREETYPE_H


FT_Library  ft_library;
char		*g_last_error = "";


typedef struct TGlyph_
{
	FT_UInt		index;
	FT_Vector	pos;
	FT_Glyph	image;

} TGlyph, *PGlyph;


static void blit_glyph(int x, int y, _pel base_col, FT_Bitmap *glyph, gimp_image_t *pmap) //<<<
{
	uint32				px, py, mx, my;
	unsigned char		*s;
	unsigned char		*ts;
	register _pel		*td;
	_pel				*d;
	_pel				pel;
	
	td = d = pmap->pixel_data + y*pmap->width + x;
	my = y + glyph->rows;
	mx = x + glyph->width;

	ts = s = glyph->buffer;

	for (py=y; py<my; py++) {
		for (px=x; px<mx; px++, ts++, td++) {
			pel = base_col;
			pel.ch.a = fact[pel.ch.a][*ts];
//			P_ALPHA(td, (&pel));
			P_BLIT(td, (&pel));
//			*td = pel;
		}
		ts = (s += glyph->pitch);
		td = (d += pmap->width);
	}
}

//>>>
pmap_list *render_ttf(_pel base_col, FT_Face face, int px_size, char *utf8_text, int wrap_width, ttf_feedback_cb cb, void *clientdata) //<<<
{
	int				n, from_glyph, glyphno, numchars, numwchars, error;
	int				pen_x, pen_y, size;
	FT_GlyphSlot	slot = face->glyph;
	FT_Bool			use_kerning;
	FT_UInt			previous;
	TGlyph			*glyphs = NULL;
	PGlyph			glyph;
	FT_BBox			bbox;
	gimp_image_t	*pmap;
	pmap_list		*reslist = NULL;
	pmap_list		*curr_pmap;
	wchar_t			this_wchar;
	char			*mbptr = utf8_text;
	int				res;
	mbstate_t		ps;
	char			*last_break = NULL;
	int				since_break = 0;

#ifdef VDEBUG
#	define ERRDUMP	fprintf(stderr, "%s offset: %d numchars: %d 0x%x 0x%x 0x%x 0x%x\n", g_last_error, mbptr - utf8_text, numchars, (unsigned char)mbptr[0], (unsigned char)mbptr[1], (unsigned char)mbptr[2], (unsigned char)mbptr[3])
#else
#	define ERRDUMP
#endif
	setlocale(LC_CTYPE, "en_GB.UTF-8");

	error = FT_Set_Pixel_Sizes(
			face,
			0,
			px_size);

	pen_x = 0;
	pen_y = 0;
	previous = 0;
	use_kerning = FT_HAS_KERNING(face);

	// Figure out actual number of (wide) characters
	numchars = strlen(utf8_text);
	memset(&ps, 0, sizeof(mbstate_t));
	numwchars = mbsrtowcs(NULL, (const char **)&mbptr, numchars, &ps);
	mbptr = utf8_text;
	memset(&ps, 0, sizeof(mbstate_t));

	glyphs = (TGlyph *)malloc(sizeof(TGlyph) * numwchars);
	glyph = glyphs;
	glyphno = 0;

	reslist = (pmap_list *)malloc(sizeof(pmap_list) * 1);
	reslist->pmap = NULL;
	reslist->next = NULL;
	reslist->clientdata = NULL;
	curr_pmap = reslist;

	while (numchars - (mbptr - utf8_text) > 0) {
		if (*mbptr == ' ') {
			last_break = mbptr;
			since_break = 0;
#ifdef VDEBUG
			fprintf(stderr, "whitespace found at %p, glyphno %d\n", last_break, glyphno);
#endif
		} else {
			since_break++;
		}
		res = mbrtowc(&this_wchar, mbptr, numchars - (mbptr - utf8_text), &ps);
		if (res < 0) {
			switch (res) {
				case -1:
					g_last_error = "No complete multibyte character while decoding input";
					ERRDUMP;
					goto error;			// See COMEFROM (Intercal)
					break;
				case -2:
					g_last_error = "Invalid multibyte character in input";
					ERRDUMP;
					goto error;
					break;
				default:
					g_last_error = "Unknown error processing input";
					ERRDUMP;
					goto error;
					break;
			}
		}
		
		if (res == 0) 
			break;			// Is this right??

		if (*mbptr == '\n') {
			pen_x = 0;
			pen_y = 0;
			previous = 0;

			curr_pmap->clientdata = glyphno-1;	// Last glyph
			curr_pmap->next = (pmap_list *)malloc(sizeof(pmap_list) * 1);
			curr_pmap = curr_pmap->next;
			curr_pmap->pmap = NULL;
			curr_pmap->next = NULL;
			curr_pmap->clientdata = NULL;
			mbptr += res;
			continue;
		}
		mbptr += res;
		
		// retrieve glyph index from character code
		glyph->index = FT_Get_Char_Index(face, this_wchar);

		if (use_kerning && previous && glyph->index) {
			FT_Vector	delta;
			
			FT_Get_Kerning(face, previous, glyph->index,
					ft_kerning_default, &delta);

			pen_x += delta.x >> 6;
		}

		glyph->pos.x = pen_x;
		glyph->pos.y = pen_y;

		error = FT_Load_Glyph(face, glyph->index, FT_LOAD_DEFAULT);
		if (error) continue;  // ignore errors

		error = FT_Get_Glyph(face->glyph, &glyph->image);
		if (error) continue;
		
		FT_Glyph_Transform(glyph->image, 0, &glyph->pos);
		
		pen_x += slot->advance.x >> 6;
		previous = glyph->index;

		glyph++;
		glyphno++;

		if (wrap_width > 0 && pen_x > wrap_width) {
#ifdef VDEBUG
			fprintf(stderr, "pen_x (%d) > wrap_width (%d), wrapping\n",
					pen_x, wrap_width);
#endif
			pen_x = 0;
			pen_y = 0;
			previous = 0;

			mbptr = last_break + 1;		// TODO: check out-of-bounds
			glyph -= since_break;
			glyphno -= since_break;
			for (n = glyphno; n < glyphno + since_break; n++)
				FT_Done_Glyph(glyphs[n].image);
			curr_pmap->clientdata = glyphno-1;	// Last glyph
#ifdef VDEBUG
			fprintf(stderr, "falling back to glyphno %d, mbptr %p\n",
					glyphno, mbptr);
#endif
			curr_pmap->next = (pmap_list *)malloc(sizeof(pmap_list) * 1);
			curr_pmap = curr_pmap->next;
			curr_pmap->pmap = NULL;
			curr_pmap->next = NULL;
			curr_pmap->clientdata = NULL;
		}
	}
	curr_pmap->clientdata = glyphno-1;

	curr_pmap = reslist;
	from_glyph = 0;
	while (curr_pmap != NULL) {
		int	last_glyph = (int)(curr_pmap->clientdata);
#ifdef VDEBUG
		fprintf(stderr, "processing line ending glyph %d\n",
				last_glyph);
#endif
		// Get bounding box {{{
		bbox.xMin = bbox.yMin = 32000;
		bbox.xMax = bbox.yMax = -32000;

		for (n = from_glyph; n<=last_glyph; n++) {
			FT_BBox		glyph_bbox;

			FT_Glyph_Get_CBox(glyphs[n].image, ft_glyph_bbox_pixels, &glyph_bbox);

			if (glyph_bbox.xMin + glyphs[n].pos.x < bbox.xMin)
				bbox.xMin = glyph_bbox.xMin + glyphs[n].pos.x;

			if (glyph_bbox.yMin + glyphs[n].pos.y < bbox.yMin)
				bbox.yMin = glyph_bbox.yMin + glyphs[n].pos.y;

			if (glyph_bbox.xMax + glyphs[n].pos.x > bbox.xMax)
				bbox.xMax = glyph_bbox.xMax + glyphs[n].pos.x;

			if (glyph_bbox.yMax + glyphs[n].pos.y > bbox.yMax)
				bbox.yMax = glyph_bbox.yMax + glyphs[n].pos.y;
		}

		if (bbox.xMin > bbox.xMax) {
			bbox.xMin = 0;
			bbox.yMin = 0;
			bbox.xMax = 0;
			bbox.yMax = 0;
		}
		// Get bounding box }}}

		//	fprintf(stderr, "bbox.yMin: (%d) bbox.xMin: (%d)\nbbox.yMax: (%d) bbox.xMax: (%d)\n", bbox.yMin, bbox.xMin, bbox.yMax, bbox.xMax);
		// Alloc pmap
		curr_pmap->pmap = pmap = (gimp_image_t *)malloc(sizeof(gimp_image_t));
		pmap->width = bbox.xMax - bbox.xMin + 1;
		pmap->height = bbox.yMax - bbox.yMin + 1;
		pmap->bytes_per_pixel = 4;
//		g_total_pmaps++;
//		fprintf(stderr, "allocing pmap: w: %d h: %d  %p\n",
//				pmap->width, pmap->height, pmap); fflush(stderr);

//		fprintf(stderr, "width: (%d) height: (%d)\n", pmap->width, pmap->height);
		size = pmap->width * pmap->height * pmap->bytes_per_pixel;
		pmap->pixel_data = (_pel *)malloc(size);
		//	memset(pmap->pixel_data, 0xc0, size);
		memset(pmap->pixel_data, 0x00, size);

		// Render to pmap
		for (n = from_glyph; n<=last_glyph; n++) {
			FT_Glyph	image;
			//		FT_Vector	pen;
			//		FT_BBox		bbox;

			// create a copy of the original glyph
			error = FT_Glyph_Copy(glyphs[n].image, &image);
			if (error) continue;

			// transform copy (this will also translate it to the correct position
			//FT_Glyph_Transform(image, &matrix, &start);

			// convert glyph image to bitmap (destroy the glyph copy !!)
			error = FT_Glyph_To_Bitmap(&image,
					ft_render_mode_normal,
					0,		// no additional translation
					1);		// destroy copy in "image"

			if (!error) {
				FT_BitmapGlyph	bitmap = (FT_BitmapGlyph)image;
				//			fprintf(stderr, "bbox.yMin: (%d) bbox.xMin: (%d)\n", bbox.yMin, bbox.xMin);
				blit_glyph(
						bitmap->left + glyphs[n].pos.x - bbox.xMin,
						(pmap->height - bitmap->top) + glyphs[n].pos.y + bbox.yMin,
						base_col,
						&bitmap->bitmap,
						pmap);
				if (cb != NULL)
					cb(clientdata, TTF_FEEDBACK_CHAR, bitmap->left + glyphs[n].pos.x - bbox.xMin);
			}
			FT_Done_Glyph(image);
		}
		if (cb != NULL)
			cb(clientdata, TTF_FEEDBACK_LINESTART, from_glyph);
		from_glyph = last_glyph + 1;
		curr_pmap = curr_pmap->next;
	}
	
	if (glyphs != NULL) {
		for (n = 0; n < glyphno; n++)
			FT_Done_Glyph(glyphs[n].image);
		free(glyphs); glyphs = NULL;
	}
	return reslist;

error:
	if (glyphs != NULL) {
		for (n = 0; n < glyphno; n++)
			FT_Done_Glyph(glyphs[n].image);
		free(glyphs);
	}
	while (reslist != NULL) {
		curr_pmap = reslist->next;
		if (reslist->pmap != NULL)
			free(reslist->pmap);
		free(reslist);
	}
	return NULL;
}


//>>>
int ttf_init_state(Tcl_Interp* interp, _pel base_col, FT_Face face, int px_size, struct ttf_state* state) //<<<
{
	int		error;

	setlocale(LC_CTYPE, "en_GB.UTF-8");

	memset(state, 0, sizeof(*state));

	error = FT_Set_Pixel_Sizes(
			face,
			0,
			px_size);

	state->advance_x = 0.0;
	state->advance_y = 0.0;
	state->previous = 0;
	state->face = face;
	state->use_kerning = FT_HAS_KERNING(face);
	state->slot = face->glyph;
	state->base_col = base_col;

	return TCL_OK;
}

//>>>
int ttf_next_char(Tcl_Interp* interp, struct ttf_state* state, const char* utf8_char, int char_byte_len, double* dx, double* dy, int* ox, int* oy, int* width, gimp_image_t** pmap) //<<<
{
	FT_Glyph		image;
	FT_BBox			glyph_bbox;
	FT_BitmapGlyph	bitmap;
	double			delta_x = state->advance_x;
	double			delta_y = state->advance_y;
	wchar_t			this_wchar;
	const char		*mbptr;
	int				res, error, size, numwchars;
	mbstate_t		ps;
	TGlyph			glyph;
	FT_BBox			bbox;

	mbptr = utf8_char;

	// Figure out actual number of (wide) characters
	memset(&ps, 0, sizeof(mbstate_t));
	numwchars = mbsrtowcs(NULL, (const char **)&mbptr, char_byte_len, &ps);
	mbptr = utf8_char;
	memset(&ps, 0, sizeof(mbstate_t));

	res = mbrtowc(&this_wchar, mbptr, char_byte_len - (mbptr - utf8_char), &ps);
	if (res < 0) {
		switch (res) {
			case -1:
				THROW_ERROR("No complete multibyte character while decoding input");
				break;
			case -2:
				THROW_ERROR("Invalid multibyte character in input");
				break;
			default:
				THROW_ERROR("Unknown error processing input");
				break;
		}
	}
	
	if (res == 0) 
		THROW_ERROR("No characters found");

	mbptr += res;
	if (mbptr < utf8_char + char_byte_len)
		fprintf(stderr, "Warning: got more utf8 data than we consumed\n");

	// retrieve glyph index from character code
	glyph.index = FT_Get_Char_Index(state->face, this_wchar);

	if (state->use_kerning && state->previous && glyph.index) {
		FT_Vector	delta;

		FT_Get_Kerning(state->face, state->previous, glyph.index,
				ft_kerning_default, &delta);

		delta_x += (double)delta.x / (1<<6);
		delta_y += (double)delta.y / (1<<6);
	}

	glyph.pos.x = (int)delta_x;
	glyph.pos.y = (int)delta_y;

	error = FT_Load_Glyph(state->face, glyph.index, FT_LOAD_DEFAULT);
	if (error) THROW_ERROR("Error loading glyph");

	error = FT_Get_Glyph(state->face->glyph, &glyph.image);
	if (error) THROW_ERROR("Error retrieving glyph image");

	FT_Glyph_Transform(glyph.image, 0, &glyph.pos);

	*dx = delta_x;
	*dy = delta_y;
	delta_x += (double)state->slot->advance.x / (1<<6);
	delta_y += (double)state->slot->advance.y / (1<<6);
	state->advance_x = (double)state->slot->advance.x / (1<<6);
	state->advance_y = (double)state->slot->advance.y / (1<<6);
	state->previous = glyph.index;

	// Get bounding box <<<
	bbox.xMin = bbox.yMin = 32000;
	bbox.xMax = bbox.yMax = -32000;

	FT_Glyph_Get_CBox(glyph.image, ft_glyph_bbox_pixels, &glyph_bbox);

	if (glyph_bbox.xMin + glyph.pos.x < bbox.xMin)
		bbox.xMin = glyph_bbox.xMin + glyph.pos.x;

	if (glyph_bbox.yMin + glyph.pos.y < bbox.yMin)
		bbox.yMin = glyph_bbox.yMin + glyph.pos.y;

	if (glyph_bbox.xMax + glyph.pos.x > bbox.xMax)
		bbox.xMax = glyph_bbox.xMax + glyph.pos.x;

	if (glyph_bbox.yMax + glyph.pos.y > bbox.yMax)
		bbox.yMax = glyph_bbox.yMax + glyph.pos.y;

	if (bbox.xMin > bbox.xMax) {
		bbox.xMin = 0;
		bbox.yMin = 0;
		bbox.xMax = 0;
		bbox.yMax = 0;
	}
	// Get bounding box >>>

	//	fprintf(stderr, "bbox.yMin: (%d) bbox.xMin: (%d)\nbbox.yMax: (%d) bbox.xMax: (%d)\n", bbox.yMin, bbox.xMin, bbox.yMax, bbox.xMax);
	// Alloc pmap
	*pmap = (gimp_image_t*)malloc(sizeof(**pmap));
	(*pmap)->width = bbox.xMax - bbox.xMin + 1;
	(*pmap)->height = bbox.yMax - bbox.yMin + 1;
	(*pmap)->bytes_per_pixel = 4;
	size = (*pmap)->width * (*pmap)->height * (*pmap)->bytes_per_pixel;
	(*pmap)->pixel_data = (_pel*)malloc(size);
	memset((*pmap)->pixel_data, 0x00, size);

	// create a copy of the original glyph
	error = FT_Glyph_Copy(glyph.image, &image);
	if (error) THROW_ERROR("Error copying glyph");

	// transform copy (this will also translate it to the correct position
	//FT_Glyph_Transform(image, &matrix, &start);

	// convert glyph image to bitmap (destroy the glyph copy !!)
	error = FT_Glyph_To_Bitmap(&image,
			ft_render_mode_normal,
			0,		// no additional translation
			1);		// destroy copy in "image"
	if (error) THROW_ERROR("Error rendering glyph bitmap");

	bitmap = (FT_BitmapGlyph)image;
	*ox = bitmap->left;
	//*oy = (*pmap)->height - bitmap->top + bbox.yMin + glyph.pos.y + (int)delta_y;
	*oy = bitmap->top + (int)delta_y;
	//fprintf(stderr, "rendering %s, bbox.yMin: %d, bbox.yMax: %d, bitmap->top: %d\n", utf8_char, bbox.yMin, bbox.yMax, bitmap->top);
	blit_glyph(0, 0,
			state->base_col,
			&bitmap->bitmap,
			*pmap);

	*width = (*pmap)->width;
	FT_Done_Glyph(image);
	
	FT_Done_Glyph(glyph.image);
	return TCL_OK;
}


//>>>
char *ttf_last_error_txt() //<<<
{
	return g_last_error;
}

//>>>
int init_ttf() //<<<
{
	return FT_Init_FreeType(&ft_library);
}

//>>>

// vim: ft=c foldmethod=marker foldmarker=<<<,>>> ts=4 shiftwidth=4
