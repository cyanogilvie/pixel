#include "tcl_utils.h"

static int _read_file_ex(interp, filename, res, binary)
	Tcl_Interp	*interp;
   	const char	*filename;
   	Tcl_Obj		**res;
   	int			binary;
{
	Tcl_Channel	fshandle;
	Tcl_Obj		*fnobj;

	fnobj = Tcl_NewStringObj(filename, -1);
	Tcl_IncrRefCount(fnobj);

	fshandle = Tcl_FSOpenFileChannel(interp, fnobj, "r", 0644);
	Tcl_DecrRefCount(fnobj);
	if (fshandle == NULL) return TCL_ERROR;
	
	if (binary) {
		TEST_OK(Tcl_SetChannelOption(interp, fshandle, "-translation", "binary"));
		TEST_OK(Tcl_SetChannelOption(interp, fshandle, "-encoding", "binary"));
	}

	TEST_OK(Tcl_SetChannelOption(interp, fshandle, "-blocking", "1"));

	*res = Tcl_NewObj();
	Tcl_ReadChars(fshandle, *res, -1, 0);
	TEST_OK(Tcl_Close(interp, fshandle));
	fshandle = NULL;

	return TCL_OK;
}


const unsigned char *read_vfs_file(const char *filename, int *len, int binary)
{
	Tcl_Obj		*res;

	if (_read_file_ex(NULL, filename, &res, binary) != TCL_OK) {
		return NULL;
	} else {
		return Tcl_GetByteArrayFromObj(res, len);
	}
}


int read_vfs_file2(interp, filename, buf, len, binary)
	Tcl_Interp		*interp;
	const char		*filename;
	unsigned char	**buf;
	int				*len;
	int				binary;
{
	Tcl_Obj		*res;

	TEST_OK(_read_file_ex(interp, filename, &res, binary));
	*buf = Tcl_GetByteArrayFromObj(res, len);

	return TCL_OK;
}


