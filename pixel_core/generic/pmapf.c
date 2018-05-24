//#include "all.h"
#include "pixel.h"

static void free_internal_rep(Tcl_Obj* obj);
static void dup_internal_rep(Tcl_Obj* src, Tcl_Obj* dest);
static void update_string_rep(Tcl_Obj* obj);
static int set_pmapf_from_any(Tcl_Interp* interp, Tcl_Obj* obj);


Tcl_ObjType pmapf_type = {
	"PMAPF",
	free_internal_rep,
	dup_internal_rep,
	update_string_rep,
	set_pmapf_from_any
};


static void free_internal_rep(Tcl_Obj* obj) //<<<
{
	struct pmapf*	pmapf = (struct pmapf*)obj->internalRep.twoPtrValue.ptr1;
	sp_info*		sp = (sp_info*)obj->internalRep.twoPtrValue.ptr2;
	//fprintf(stderr, "tcl_pmap: Called free_internal_rep\n");

#warning pmap free_internal rep must free sp_info
	//call sp->free_info(obj) routine

	if (pmapf != NULL) {
		pmapf_free(&pmapf);
	}
}

//>>>
static void dup_internal_rep(Tcl_Obj* src, Tcl_Obj* dest) //<<<
{
	struct pmapf*	src_pmapf = (struct pmapf*)src->internalRep.twoPtrValue.ptr1;
	struct pmapf*	dest_pmapf;
	int				size;

//	fprintf(stderr, "tcl_pmap: Called dup_internal_rep\n");

	size = src_pmapf->width * src_pmapf->height * src_pmapf->bytes_per_pixel;
	dest_pmapf = pmapf_new(src_pmapf->width, src_pmapf->height);

	memcpy(dest_pmapf->pixel_data, src_pmapf->pixel_data, size);
	
	dest->typePtr = &pmapf_type;
	dest->internalRep.twoPtrValue.ptr1 = dest_pmapf;
	dest->internalRep.twoPtrValue.ptr2 = NULL;
}

//>>>
static void update_string_rep(Tcl_Obj* obj) //<<<
{
	struct pmapf*	pmapf = (struct pmapf*)obj->internalRep.twoPtrValue.ptr1;
	Tcl_Obj*		objv[4];
	Tcl_Obj*		list = NULL;
	char*			str;
	int				length;

	//fprintf(stderr, "tcl_pmap: Called update_string_rep\n");
	//Tcl_Panic("Bang");
	
	objv[0] = Tcl_NewIntObj(pmapf->width);
	objv[1] = Tcl_NewIntObj(pmapf->height);
	objv[2] = Tcl_NewIntObj(pmapf->bytes_per_pixel);
	objv[3] = Tcl_NewByteArrayObj((unsigned char *)pmapf->pixel_data,
			pmapf->width *
			pmapf->height *
			pmapf->bytes_per_pixel);

	Tcl_IncrRefCount(list = Tcl_NewListObj(4, objv));
	
	str = Tcl_GetStringFromObj(list, &length);
	
	obj->bytes = ckalloc(length + 1);
	memcpy(obj->bytes, str, length);
	obj->bytes[length] = 0;
	obj->length = length;

	Tcl_InvalidateStringRep(list);
	Tcl_DecrRefCount(list); list = NULL;

	//fprintf(stderr, "rep: (%s)\n", Tcl_GetString(obj));
}

//>>>
static int set_pmapf_from_any(Tcl_Interp* interp, Tcl_Obj* obj) //<<<
{
	const Tcl_ObjType*	oldtype = obj->typePtr;
	struct pmapf*	pmapf;
	int				objc;
	Tcl_Obj**		objv;
	int				width, height, bytes_per_pixel;
	pelf*			pixel_data;
	int				size, src_size;

	//fprintf(stderr, "tcl_pmap: Called set_pmap_from_any: (%s)\n", Tcl_GetString(obj));
	//THROW_ERROR("Bang");
	//fprintf(stderr, "tcl_pmap: Called set_pmap_from_any: current type: (%s), already pmap? (%d)\n", obj->typePtr->name, oldtype == &tcl_pmap);

	//fprintf(stderr, "pmap foo\n");
	if (oldtype == &pmapf_type)
		return TCL_OK;

	//fprintf(stderr, "pmap bar\n");
	TEST_OK(Tcl_ListObjGetElements(interp, obj, &objc, &objv));
	//fprintf(stderr, "pmap baz\n");

	if (objc != 4)
		THROW_ERROR("PMAPF expects a 4 element list: width height bytes_per_pixel pixel_data");

	TEST_OK(Tcl_GetIntFromObj(interp, objv[0], &width));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[1], &height));
	TEST_OK(Tcl_GetIntFromObj(interp, objv[2], &bytes_per_pixel));

	if (width < 0) THROW_ERROR("width cannot be less than 0");
	if (height < 0) THROW_ERROR("height cannot be less than 0");
	if (bytes_per_pixel != sizeof(pelf))
		THROW_ERROR("can only deal with 16 bytes_per_pixel currently");

	size = width * height * bytes_per_pixel;
	pixel_data = (pelf*)Tcl_GetByteArrayFromObj(objv[3], &src_size);

	if (size != src_size) {
		fprintf(stderr, "Size mismatch: %d should be %d\n", src_size, size);
		THROW_ERROR("Supplied pixel data is the wrong size");
	}

	pmapf = pmapf_new(width, height);
	memcpy(pmapf->pixel_data, pixel_data, size);

	if (oldtype != NULL && oldtype->freeIntRepProc != NULL)
		oldtype->freeIntRepProc(obj);

	obj->typePtr = &pmapf_type;
	obj->internalRep.twoPtrValue.ptr1 = pmapf;
	obj->internalRep.twoPtrValue.ptr2 = NULL;
	Tcl_InvalidateStringRep(obj);

	return TCL_OK;
}

//>>>

void Pixel_SetPMAPFObj(Tcl_Obj* obj, struct pmapf* pmapf) //<<<
{
	const Tcl_ObjType*	oldType = obj->typePtr;

//	fprintf(stderr, "tcl_pmap: Called Tcl_SetPMAPObj\n");

	if (Tcl_IsShared(obj))
		panic("Tcl_SetPMAPFObj called with shared obj");

	if ((oldType != NULL) && (oldType->freeIntRepProc != NULL))
		oldType->freeIntRepProc(obj);

	obj->typePtr = &pmapf_type;
	obj->internalRep.twoPtrValue.ptr1 = pmapf;
	obj->internalRep.twoPtrValue.ptr2 = NULL;
	Tcl_InvalidateStringRep(obj);
}

//>>>
Tcl_Obj* Pixel_NewPMAPFObj(struct pmapf* pmapf) //<<<
{
	Tcl_Obj*	new;
	
//	fprintf(stderr, "tcl_pmap: Called Tcl_NewPMAPObj\n");

	if (pmapf == NULL)
		Tcl_Panic("Got a NULL pmapf to wrap in a Tcl_Obj, refusing\n");

	new = Tcl_NewObj();
	Pixel_SetPMAPFObj(new, pmapf);

	return new;
}

//>>>
int Pixel_GetPMAPFFromObj(Tcl_Interp* interp, Tcl_Obj* obj, struct pmapf** pmapf) //<<<
{
//	fprintf(stderr, "tcl_pmap: Called Tcl_GetPMAPFromObj\n");

	if (obj->typePtr != &pmapf_type)
		TEST_OK(set_pmapf_from_any(interp, obj));

	*pmapf = (struct pmapf*)obj->internalRep.twoPtrValue.ptr1;
	if (*pmapf == NULL)
		THROW_ERROR("pmapf internal rep is NULL!");

	return TCL_OK;
}

//>>>
int Pixel_IsPMAPFObj(Tcl_Obj* obj) //<<<
{
	return (obj->typePtr == &pmapf_type);
}

//>>>
// vim: foldmethod=marker foldmarker=<<<,>>> ts=4 shiftwidth=4
