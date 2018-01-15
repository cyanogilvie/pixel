#ifndef _PMAPF_H
#define _PMAPF_H

EXTERN_C Tcl_ObjType pmapf_type;

EXTERN_C Tcl_Obj *Pixel_NewPMAPFObj(struct pmapf* pmapf);
EXTERN_C int Pixel_GetPMAPFFromObj(Tcl_Interp* interp, Tcl_Obj* obj,
		struct pmapf** pmapf);
EXTERN_C int Pixel_IsPMAPFObj(Tcl_Obj* obj);

struct pmapf* pmap_to_pmapf(gimp_image_t* pmap);
gimp_image_t* pmapf_to_pmap(struct pmapf* pmapf);
struct pmapf* pmapf_new(int width, int height);
void pmapf_free(struct pmapf** pmapf);

#endif
