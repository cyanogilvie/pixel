#include <tclInt.h>
#include <tclstuff.h>
#include "ttf.h"

static void free_internal_rep(Tcl_Obj *obj);
static void update_string_rep(Tcl_Obj *obj);
static int set_ttf_face_from_any(Tcl_Interp *interp, Tcl_Obj *obj);


Tcl_ObjType tcl_ttf_face = {
	"TTFFace",
	free_internal_rep,
	(Tcl_DupInternalRepProc *)NULL,
	update_string_rep,
	set_ttf_face_from_any
};


static void free_internal_rep(Tcl_Obj *obj)
{
	FT_Face		face = (FT_Face)obj->internalRep.twoPtrValue.ptr1;
	char *		filename = (char *)obj->internalRep.twoPtrValue.ptr2;

	fprintf(stderr, "tcl_ttf_face: Called free_internal_rep\n");

	FT_Done_Face(face);
	free(filename);
}


/*
static void dup_internal_rep(Tcl_Obj *src, Tcl_Obj *dest)
{
	FT_Face		face = (FT_Face *)obj->internalRep.twoPtrValue.ptr1;
	gimp_image_t *	src_pmap = (gimp_image_t *)src->internalRep.otherValuePtr;
	gimp_image_t *	dest_pmap;

	fprintf(stderr, "tcl_ttf_face: Called dup_internal_rep\n");

	dest_pmap = malloc(sizeof(gimp_image_t));
	dest_pmap->pixel_data = (_pel *)malloc(
			src_pmap->width * 
			src_pmap->height *
			src_pmap->bytes_per_pixel);
	
	dest->internalRep.otherValuePtr = dest_pmap;
}
*/


static void update_string_rep(Tcl_Obj * obj)
{
	FT_Face			face = (FT_Face)obj->internalRep.twoPtrValue.ptr1;
	Tcl_Obj *		objv[2];
	Tcl_Obj *		list;
	char *			str;
	int				length;

//	fprintf(stderr, "tcl_ttf_face: Called update_string_rep\n");
	
	objv[0] = Tcl_NewStringObj((char *)obj->internalRep.twoPtrValue.ptr2, -1);
	objv[1] = Tcl_NewIntObj(face->face_index);

	list = Tcl_NewListObj(2, objv);
	
	str = Tcl_GetStringFromObj(list, &length);
	
	obj->bytes = Tcl_Alloc(length + 1);
	memcpy(obj->bytes, str, length);
	obj->bytes[length] = 0;
	obj->length = length;

//	fprintf(stderr, "rep: (%s)\n", Tcl_GetString(obj));
}


static int set_ttf_face_from_any(Tcl_Interp *interp, Tcl_Obj *obj)
{
	Tcl_ObjType *	oldtype = obj->typePtr;
	FT_Face			face;
	int				objc;
	Tcl_Obj **		objv;
	char *			filename;
	int				face_index, error;
	
	fprintf(stderr, "tcl_ttf_face: Called set_pmap_from_any\n");

	if (oldtype == &tcl_ttf_face)
		return TCL_OK;

	TEST_OK(Tcl_ListObjGetElements(interp, obj, &objc, &objv));
	
	if (objc < 1 || objc > 2)
		THROW_ERROR("TTFFace expects a list of: filename ?face_index?");

	filename = Tcl_GetString(objv[0]);
	if (objc == 2) {
		TEST_OK(Tcl_GetIntFromObj(interp, objv[1], &face_index));
	} else {
		face_index = 0;
	}

	error = FT_New_Face(ft_library, filename, face_index, &face);

	if (error == FT_Err_Unknown_File_Format) {
		THROW_ERROR("Unknown file format opening ttf file ", filename);
	} else if (error) {
		THROW_ERROR("Could not load ttf file ", filename);
	}
	
	if (oldtype != NULL && oldtype->freeIntRepProc != NULL)
		oldtype->freeIntRepProc(obj);

	obj->typePtr = &tcl_ttf_face;
	obj->internalRep.twoPtrValue.ptr1 = face;
	obj->internalRep.twoPtrValue.ptr2 = filename;
	Tcl_InvalidateStringRep(obj);

	return TCL_OK;
}


void Tcl_SetTTFFaceObj(Tcl_Obj * obj, FT_Face face, char * filename)
{
	Tcl_ObjType *	oldType = obj->typePtr;

//	fprintf(stderr, "tcl_ttf_face: Called Tcl_SetTTFFaceObj\n");

	if (Tcl_IsShared(obj))
		panic("Tcl_SetTTFFaceObj called with shared obj");

	if ((oldType != NULL) && (oldType->freeIntRepProc != NULL))
		oldType->freeIntRepProc(obj);

	obj->typePtr = &tcl_ttf_face;
	obj->internalRep.twoPtrValue.ptr1 = face;
	obj->internalRep.twoPtrValue.ptr2 = strdup(filename);
	Tcl_InvalidateStringRep(obj);
}


Tcl_Obj *Tcl_NewTTFFaceObj(FT_Face face, char * filename)
{
	Tcl_Obj *	new;
	
//	fprintf(stderr, "tcl_ttf_face: Called Tcl_NewTTFFaceObj\n");

	new = Tcl_NewObj();
	Tcl_SetTTFFaceObj(new, face, filename);

	return new;
}


int Tcl_GetTTFFaceFromObj(Tcl_Interp * interp, Tcl_Obj * obj, FT_Face * face)
{
//	fprintf(stderr, "tcl_ttf_face: Called Tcl_GetTTFFaceFromObj\n");

	if (obj->typePtr != &tcl_ttf_face)
		TEST_OK(set_ttf_face_from_any(interp, obj));

	*face = (FT_Face)obj->internalRep.twoPtrValue.ptr1;

	return TCL_OK;
}


