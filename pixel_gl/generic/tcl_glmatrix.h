#ifndef _TCL_GLMATRIX_H
#define _TCL_GLMATRIX_H

#include <tcl.h>
#include <GL/gl.h>

#ifndef EXTERN_C
#	ifdef _cplusplus
#		define EXTERN_C extern "C"
#	else
#		define EXTERN_C extern
#	endif
#endif

EXTERN_C Tcl_ObjType tcl_glMatrix;

EXTERN_C int Tcl_GetGlMatrixFromObj(Tcl_Interp* interp, Tcl_Obj* obj, GLfloat** m);
EXTERN_C Tcl_Obj* Tcl_NewGlMatrixObj(GLfloat* m);

#endif
