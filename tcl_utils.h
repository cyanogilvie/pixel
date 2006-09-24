#ifndef _TCL_UTILS_H
#define _TCL_UTILS_H

/*
 * This is a set of utility functions using the Tcl library, ie. for reading
 * data from the Tcl VFS system
 */

#include <tclstuff.h>

const char *read_vfs_file(const char *filename, int *len, int binary);
int read_vfs_file2(Tcl_Interp *interp, const char *filename, char **buf, int *len, int binary);

#endif
