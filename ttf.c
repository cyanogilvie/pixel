#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>
#include <unistd.h>
#include "2d.h"
#include "ttf.h"


FT_Library  ft_library;
//FT_Face		face;


typedef struct TGlyph_
{
	FT_UInt		index;
	FT_Vector	pos;
	FT_Glyph	image;

} TGlyph, *PGlyph;



static void blit_glyph(int x, int y, _pel base_col, FT_Bitmap *glyph,
		gimp_image_t *pmap)
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


#define MAX_GLYPHS		1000
gimp_image_t *render_ttf(_pel base_col, FT_Face face, int px_size, char *text)
{
	int				i, n, numchars, error, pen_x, pen_y, size;
	FT_GlyphSlot	slot = face->glyph;
	FT_Bool			use_kerning;
	FT_UInt			previous;
	TGlyph			glyphs[MAX_GLYPHS];
	PGlyph			glyph;
	FT_UInt			num_glyphs;
	FT_BBox			bbox;
	gimp_image_t *	pmap;

	error = FT_Set_Pixel_Sizes(
			face,
			0,
			px_size);

	pen_x = 0;
	pen_y = 0;
	num_glyphs = 0;
	use_kerning = FT_HAS_KERNING(face);
	previous = 0;

	glyph = glyphs;

	numchars = strlen(text);

	for (i=0; i<numchars; i++) {
		// retrieve glyph index from character code
		glyph->index = FT_Get_Char_Index(face, text[i]);

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
	}

	num_glyphs = glyph - glyphs;

	// Get bounding box {{{
	bbox.xMin = bbox.yMin = 32000;
	bbox.xMax = bbox.yMax = -32000;

	for (n = 0; n<num_glyphs; n++) {
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
	pmap = (gimp_image_t *)malloc(sizeof(gimp_image_t));
	pmap->width = bbox.xMax - bbox.xMin + 1;
	pmap->height = bbox.yMax - bbox.yMin + 1;
	pmap->bytes_per_pixel = 4;
//	fprintf(stderr, "width: (%d) height: (%d)\n", pmap->width, pmap->height);
	size = pmap->width * pmap->height * pmap->bytes_per_pixel;
	pmap->pixel_data = (_pel *)malloc(size);
//	memset(pmap->pixel_data, 0xc0, size);
	memset(pmap->pixel_data, 0x00, size);
	
	// Render to pmap
	for (n = 0; n<num_glyphs; n++) {
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
				0);		// don't destroy copy in "image"

		if (!error) {
			FT_BitmapGlyph	bitmap = (FT_BitmapGlyph)image;
//			fprintf(stderr, "bbox.yMin: (%d) bbox.xMin: (%d)\n", bbox.yMin, bbox.xMin);
			blit_glyph(
					bitmap->left + glyphs[n].pos.x - bbox.xMin,
					(pmap->height - bitmap->top) + glyphs[n].pos.y + bbox.yMin,
					base_col,
					&bitmap->bitmap,
					pmap);
			FT_Done_Glyph(image);
		}
	}
	
	// Return pmap
	return pmap;
}


int init_ttf()
{
	return FT_Init_FreeType(&ft_library);
}


