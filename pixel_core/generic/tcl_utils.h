#ifndef _TCL_UTILS_H
#define _TCL_UTILS_H

/*
 * This is a set of utility functions using the Tcl library, ie. for reading
 * data from the Tcl VFS system
 */

#include "tclstuff.h"

/* Tcl_Size compat for Tcl 8 — mirrors the guard in pixel.h */
#ifndef TCL_SIZE_MAX
typedef int Tcl_Size;
#endif

const unsigned char *read_vfs_file(const char *filename, Tcl_Size *len, int binary);
int read_vfs_file2(Tcl_Interp *interp, const char *filename, unsigned char **buf, Tcl_Size *len, int binary);

#endif
