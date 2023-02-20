#include "digest_type.h"
#include "tclstuff.h"

static void free_internal_rep(Tcl_Obj* obj);
static void dup_internal_rep(Tcl_Obj* src, Tcl_Obj* dest);
static void update_string_rep(Tcl_Obj* obj);
static int set_digest_from_any(Tcl_Interp* interp, Tcl_Obj* obj);

Tcl_ObjType digest_type = {
	"pHashDigest",
	free_internal_rep,
	dup_internal_rep,
	update_string_rep,
	set_digest_from_any
};

static void free_internal_rep(Tcl_Obj* obj) //{{{
{
	Digest*		dig = (Digest*)obj->internalRep.twoPtrValue.ptr1;

	if (dig != NULL) {
		if (dig->id) {
			ckfree(dig->id);
			dig->id = NULL;
		}
		if (dig->coeffs) {
			ckfree(dig->coeffs);
			dig->coeffs = NULL;
		}
		ckfree(dig);
		dig = NULL;
	}
}

//}}}
static void _dup_digest(Digest** dest, Digest* src) //{{{
{
	*dest = (Digest*)ckalloc(sizeof(Digest));
	(*dest)->id = (char*)ckalloc(strlen(src->id)+1);
	strcpy((*dest)->id, src->id);
	(*dest)->coeffs = (uint8_t*)ckalloc(sizeof(uint8_t) * src->size);
	memcpy((*dest)->coeffs, src->coeffs, sizeof(uint8_t) * src->size);
	(*dest)->size = src->size;
}

//}}}
static void dup_internal_rep(Tcl_Obj* src, Tcl_Obj* dest) //{{{
{
	Digest*		s_dig = (Digest*)src->internalRep.twoPtrValue.ptr1;

	dest->typePtr = &digest_type;
	_dup_digest((Digest**)&dest->internalRep.twoPtrValue.ptr1, s_dig);
	dest->internalRep.twoPtrValue.ptr2 = NULL;
}

//}}}
static void update_string_rep(Tcl_Obj* obj) //{{{
{
	Digest*		dig = (Digest*)obj->internalRep.twoPtrValue.ptr1;
	Tcl_Obj*	tmp = Tcl_NewObj();
	Tcl_Obj*	coeffs = Tcl_NewObj();
	int			i, len;
	char*		str;

	Tcl_IncrRefCount(tmp);
	Tcl_ListObjAppendElement(NULL, tmp, Tcl_NewStringObj(dig->id, -1));
	for (i=0; i<dig->size; i++)
		Tcl_ListObjAppendElement(NULL, coeffs, Tcl_NewIntObj(dig->coeffs[i]));
	Tcl_ListObjAppendElement(NULL, tmp, coeffs);

	str = Tcl_GetStringFromObj(tmp, &len);

	obj->bytes = (char*)ckalloc(len+1);
	memcpy(obj->bytes, str, len);
	obj->bytes[len] = 0;
	obj->length = len;

	Tcl_DecrRefCount(tmp); tmp = NULL;
}

//}}}
static int set_digest_from_any(Tcl_Interp* interp, Tcl_Obj* obj) //{{{
{
	int			objc, coeffc, i, id_len, ret=TCL_OK;
	Tcl_Obj**	objv;
	Tcl_Obj**	coeffv;
	Digest*		dig = NULL;
	const char*	id;

	if (obj->typePtr == &digest_type)
		return TCL_OK;

	TEST_OK(Tcl_ListObjGetElements(interp, obj, &objc, &objv));

	if (objc != 2)
		THROW_ERROR("Digest expects a 2 element list: id coeffs");

	id = Tcl_GetStringFromObj(objv[0], &id_len);

	dig = (Digest*)ckalloc(sizeof(Digest));
	memset(dig, 0, sizeof(Digest));
	dig->id = (char*)ckalloc(id_len+1);
	memcpy(dig->id, id, id_len);
	dig->id[id_len] = 0;
	dig->coeffs = (uint8_t*)ckalloc(sizeof(uint8_t) * dig->size);
	TEST_OK_LABEL(err, ret, Tcl_ListObjGetElements(interp, objv[1], &coeffc, &coeffv));
	dig->size = coeffc;
	for (i=0; i<dig->size; i++) {
		int	v;
		TEST_OK_LABEL(err, ret, Tcl_GetIntFromObj(interp, coeffv[i], &v));
		dig->coeffs[i] = v;
	}

	if (obj->typePtr != NULL && obj->typePtr->freeIntRepProc != NULL)
		obj->typePtr->freeIntRepProc(obj);

	obj->typePtr = &digest_type;
	obj->internalRep.twoPtrValue.ptr1 = dig;
	obj->internalRep.twoPtrValue.ptr2 = NULL;
	Tcl_InvalidateStringRep(obj);

	return ret;

err:
	if (dig) {
		if (dig->id) {
			ckfree(dig->id);
			dig->id = NULL;
		}
		if (dig->coeffs) {
			ckfree(dig->coeffs);
			dig->coeffs = NULL;
		}
		ckfree(dig); dig = NULL;
	}

	return ret;
}

//}}}
Tcl_Obj* Pixel_NewPHashDigestObj(Digest* dig) //{{{
{
	Tcl_Obj*	obj = Tcl_NewObj();

	obj->typePtr = &digest_type;
	_dup_digest((Digest**)&obj->internalRep.twoPtrValue.ptr1, dig);
	obj->internalRep.twoPtrValue.ptr2 = NULL;
	Tcl_InvalidateStringRep(obj);

	return obj;
}

//}}}
int Pixel_GetPHashDigestFromObj(Tcl_Interp* interp, Tcl_Obj* obj, Digest** dig) //{{{
{
	if (obj->typePtr != &digest_type)
		TEST_OK(set_digest_from_any(interp, obj));

	*dig = (Digest*)obj->internalRep.twoPtrValue.ptr1;

	return TCL_OK;
}

//}}}

// vim: ts=4 shiftwidth=4 foldmethod=marker foldmarker={{{,}}}
