#ifndef _TTF_H
#define _TTF_H

#include "2d.h"
#include <tcl.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

EXTERN_C FT_Library	ft_library;
EXTERN_C Tcl_ObjType	tcl_ttf_face;

EXTERN_C pmap_list *render_ttf(_pel base_col, FT_Face face, int px_size, char *utf8_text, int wrap_width);
EXTERN_C int new_face(char *ttf_file, int px_height);
EXTERN_C int init_ttf();

EXTERN_C void Tcl_SetTTFFaceObj(Tcl_Obj * obj, FT_Face face, char * filename);
EXTERN_C Tcl_Obj *Tcl_NewTTFFaceObj(FT_Face face, char * filename);
EXTERN_C int Tcl_GetTTFFaceFromObj(Tcl_Interp * interp, Tcl_Obj * obj, FT_Face * face);
EXTERN_C char *ttf_last_error_txt();


#endif


