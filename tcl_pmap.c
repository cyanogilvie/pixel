#include <tclInt.h>
#include <tclstuff.h>
#include "tcl_pmap.h"
#include "2d.h"

static void free_internal_rep(Tcl_Obj *obj);
static void dup_internal_rep(Tcl_Obj *src, Tcl_Obj *dest);
static void update_string_rep(Tcl_Obj *obj);
static int set_pmap_from_any(Tcl_Interp *interp, Tcl_Obj *obj);


Tcl_ObjType tcl_pmap = {
	"PMAP",
	free_internal_rep,
	dup_internal_rep,
	update_string_rep,
	set_pmap_from_any
};


static void free_internal_rep(Tcl_Obj *obj)
{
	gimp_image_t *	pmap = (gimp_image_t *)obj->internalRep.twoPtrValue.ptr1;
	sp_info *		sp = (sp_info *)obj->internalRep.twoPtrValue.ptr2;
//	fprintf(stderr, "tcl_pmap: Called free_internal_rep\n");

#warning pmap free_internal rep must free sp_info
	//call sp->free_info(obj) routine

	if (pmap != NULL)
		pmap_free(&pmap);
}


static void dup_internal_rep(Tcl_Obj *src, Tcl_Obj *dest)
{
	gimp_image_t *	src_pmap = (gimp_image_t *)src->internalRep.twoPtrValue.ptr1;
	gimp_image_t *	dest_pmap;
	int				size;

//	fprintf(stderr, "tcl_pmap: Called dup_internal_rep\n");

	size = src_pmap->width * src_pmap->height * src_pmap->bytes_per_pixel;
	dest_pmap = malloc(sizeof(gimp_image_t));
	dest_pmap->pixel_data = (_pel *)malloc(size);

	dest_pmap->width = src_pmap->width;
	dest_pmap->height = src_pmap->height;
	dest_pmap->bytes_per_pixel = src_pmap->bytes_per_pixel;
	memcpy(dest_pmap->pixel_data, src_pmap->pixel_data, size);
	
	dest->typePtr = &tcl_pmap;
	dest->internalRep.twoPtrValue.ptr1 = dest_pmap;
	dest->internalRep.twoPtrValue.ptr2 = NULL;
}


static void update_string_rep(Tcl_Obj * obj)
{
	gimp_image_t *	pmap = (gimp_image_t *)obj->internalRep.twoPtrValue.ptr1;
	Tcl_Obj *		objv[4];
	Tcl_Obj *		list;
	char *			str;
	int				length;

//	fprintf(stderr, "tcl_pmap: Called update_string_rep\n");
	
	objv[0] = Tcl_NewIntObj(pmap->width);
	objv[1] = Tcl_NewIntObj(pmap->height);
	objv[2] = Tcl_NewIntObj(pmap->bytes_per_pixel);
	objv[3] = Tcl_NewByteArrayObj((unsigned char *)pmap->pixel_data, 
			pmap->width *
			pmap->height *
			pmap->bytes_per_pixel);

	list = Tcl_NewListObj(4, objv);
	
	str = Tcl_GetStringFromObj(list, &length);
	
	obj->bytes = Tcl_Alloc(length + 1);
	memcpy(obj->bytes, str, length);
	obj->bytes[length] = 0;
	obj->length = length;

//	fprintf(stderr, "rep: (%s)\n", Tcl_GetString(obj));
}


static int set_pmap_from_any(Tcl_Interp *interp, Tcl_Obj *obj)
{
	Tcl_ObjType *	oldtype = obj->typePtr;
	gimp_image_t *	pmap;
	int				objc;
	Tcl_Obj **		objv;
	int				width, height, bytes_per_pixel;
	_pel *			pixel_data;
	int				size, src_size;
	
	fprintf(stderr, "tcl_pmap: Called set_pmap_from_any: (%s)\n", Tcl_GetString(obj));

	if (oldtype == &tcl_pmap)
		return TCL_OK;

	TEST_OK(Tcl_ListObjGetElements(interp, obj, &objc, &objv));
	
	if (objc != 4)
		THROW_ERROR("PMAP expects a 4 element list: width height bytes_per_pixel pixel_data");

	TEST_OK(Tcl_GetIntFromObj(interp, objv[0], &width));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[1], &height));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[2], &bytes_per_pixel));

	if (width < 0) THROW_ERROR("width cannot be less than 0");
	if (height < 0) THROW_ERROR("height cannot be less than 0");
	if (bytes_per_pixel != 4)
		THROW_ERROR("can only deal with 4 bytes_per_pixel currently");

	size = width * height * bytes_per_pixel;
	pixel_data = (_pel *)Tcl_GetByteArrayFromObj(objv[3], &src_size);
	
	if (size != src_size) {
		fprintf(stderr, "Size mismatch: %d should be %d\n", src_size, size);
		THROW_ERROR("Supplied pixel data is the wrong size");
	}
	
	pmap = (gimp_image_t *)malloc(sizeof(gimp_image_t));
	pmap->width = width;
	pmap->height = height;
	pmap->bytes_per_pixel = bytes_per_pixel;
	pmap->pixel_data = (_pel *)malloc(size);
	memcpy(pmap->pixel_data, pixel_data, size);
	
	if (oldtype != NULL && oldtype->freeIntRepProc != NULL)
		oldtype->freeIntRepProc(obj);

	obj->typePtr = &tcl_pmap;
	obj->internalRep.twoPtrValue.ptr1 = pmap;
	obj->internalRep.twoPtrValue.ptr2 = NULL;
	Tcl_InvalidateStringRep(obj);

	return TCL_OK;
}


void Tcl_SetPMAPObj(Tcl_Obj * obj, gimp_image_t * pmap)
{
	Tcl_ObjType *	oldType = obj->typePtr;

//	fprintf(stderr, "tcl_pmap: Called Tcl_SetPMAPObj\n");

	if (Tcl_IsShared(obj))
		panic("Tcl_SetPMAPObj called with shared obj");

	if ((oldType != NULL) && (oldType->freeIntRepProc != NULL))
		oldType->freeIntRepProc(obj);

	obj->typePtr = &tcl_pmap;
	obj->internalRep.twoPtrValue.ptr1 = pmap;
	obj->internalRep.twoPtrValue.ptr2 = NULL;
	Tcl_InvalidateStringRep(obj);
}


Tcl_Obj *Tcl_NewPMAPObj(gimp_image_t * pmap)
{
	Tcl_Obj *	new;
	
//	fprintf(stderr, "tcl_pmap: Called Tcl_NewPMAPObj\n");

	new = Tcl_NewObj();
	Tcl_SetPMAPObj(new, pmap);

	return new;
}


int Tcl_GetPMAPFromObj(Tcl_Interp * interp, Tcl_Obj * obj, gimp_image_t ** pmap)
{
//	fprintf(stderr, "tcl_pmap: Called Tcl_GetPMAPFromObj\n");

	if (obj->typePtr != &tcl_pmap)
		TEST_OK(set_pmap_from_any(interp, obj));

	*pmap = (gimp_image_t *)obj->internalRep.twoPtrValue.ptr1;

	return TCL_OK;
}


