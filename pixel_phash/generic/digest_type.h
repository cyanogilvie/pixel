#ifndef _DIGEST_TYPE_H
#define _DIGEST_TYPE

#include <tcl.h>
#include <pHash.h>

extern "C" int Pixel_GetPHashDigestFromObj(Tcl_Interp* interp, Tcl_Obj* obj, Digest** digest);
extern "C" Tcl_Obj* Pixel_NewPHashDigestObj(Digest* digest);
extern "C" Tcl_ObjType digest_type;

#endif
