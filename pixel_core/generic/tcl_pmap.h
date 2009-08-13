#ifndef _TCL_PMAP_H
#define _TCL_PMAP_H

EXTERN_C Tcl_ObjType tcl_pmap;

EXTERN_C void Tcl_SetPMAPObj(Tcl_Obj * obj, gimp_image_t * pmap);
EXTERN_C Tcl_Obj *Tcl_NewPMAPObj(gimp_image_t * pmap);
EXTERN_C int Tcl_GetPMAPFromObj(Tcl_Interp * interp, Tcl_Obj * obj,
		gimp_image_t ** pmap);

#endif


