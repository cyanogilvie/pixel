#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include "tclstuff.h"
#include "tcl_glmatrix.h"
#include <string.h>

static void free_internal_rep(Tcl_Obj *obj);
static void dup_internal_rep(Tcl_Obj *src, Tcl_Obj *dest);
static void update_string_rep(Tcl_Obj *obj);
static int set_glMatrix_from_any(Tcl_Interp *interp, Tcl_Obj *obj);

Tcl_ObjType tcl_glMatrix = {
	"glMatrix",
	free_internal_rep,
	dup_internal_rep,
	update_string_rep,
	set_glMatrix_from_any
};


static void free_internal_rep(Tcl_Obj *obj) //<<<
{
	ckfree(obj->internalRep.otherValuePtr);
	obj->internalRep.otherValuePtr = NULL;
}

//>>>
static void dup_internal_rep(Tcl_Obj *src, Tcl_Obj *dest) //<<<
{
	float*		src_m = (float *)src->internalRep.otherValuePtr;
	float*		dest_m;

	dest_m = (float *)ckalloc(sizeof(float) * 16);
	memcpy(dest_m, src_m, sizeof(float) * 16);

	dest->typePtr = &tcl_glMatrix;
	dest->internalRep.otherValuePtr = dest_m;
}

//>>>
static void update_string_rep(Tcl_Obj *obj) //<<<
{
	float*		m = (float *)obj->internalRep.otherValuePtr;
	char*		strrep;
	int			len;

	len = asprintf(&strrep, 
			"%f %f %f %f\n"
			"%f %f %f %f\n"
			"%f %f %f %f\n"
			"%f %f %f %f",
			m[0],	m[1],	m[2],	m[3],
			m[4],	m[5],	m[6],	m[7],
			m[8],	m[9],	m[10],	m[11],
			m[12],	m[13],	m[14],	m[15]);
	strrep[len] = 0;

	if (len < 0) {
		Tcl_Panic("Failed to produce string rep of glMatrix, perhaps becausememory could not be allocated");
	}

	// Mem must be allocated for obj->bytes by ckalloc or Tcl_Alloc, so we have
	// to copy here :(
	// Tcl guarantees that obj->bytes will be NULL when it calls us.
	obj->bytes = ckalloc(len);
	memcpy(obj->bytes, strrep, len);
	free(strrep);
}

//>>>
static int set_glMatrix_from_any(Tcl_Interp *interp, Tcl_Obj *obj) //<<<
{
	Tcl_ObjType*	oldtype = obj->typePtr;
	int				objc, i;
	Tcl_Obj**		objv;
	float			m[16];
	float*			obj_m;
	double			staging;

	if (oldtype == &tcl_glMatrix)
		return TCL_OK;

	TEST_OK(Tcl_ListObjGetElements(interp, obj, &objc, &objv));

	if (objc != 16)
		THROW_ERROR("glMatrix expects a list of 16 floats");

	for (i=0; i<16; i++) {
		TEST_OK(Tcl_GetDoubleFromObj(interp, objv[i], &staging));
		m[i] = staging;
	}

	obj_m = (float *)ckalloc(sizeof(float) * 16);
	memcpy(obj_m, &m, sizeof(float) * 16);

	obj->typePtr = &tcl_glMatrix;
	obj->internalRep.otherValuePtr = obj_m;
	Tcl_InvalidateStringRep(obj);

	return TCL_OK;
}

//>>>

int Tcl_GetGlMatrixFromObj(Tcl_Interp* interp, Tcl_Obj* obj, float **m) //<<<
{
	if (obj->typePtr != &tcl_glMatrix)
		TEST_OK(set_glMatrix_from_any(interp, obj));

	*m = (float *)obj->internalRep.otherValuePtr;

	return TCL_OK;
}

//>>>

// vim: foldmethod=marker foldmarker=<<<,>>> ts=4 shiftwidth=4
