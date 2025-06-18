/*
 * tclUtf8Proc.h --
 *
 *	Common header file for the extension.
 *
 * Copyright (c) Ashok P. Nadkarni
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 */

#ifndef _TCLUTF8PROC_H
#define _TCLUTF8PROC_H

#include "utf8proc.h"
#include <tcl.h>

#if (TCL_MAJOR_VERSION < 8) || (TCL_MAJOR_VERSION == 8 && TCL_MINOR_VERSION < 7)
# undef Tcl_Size
  typedef int Tcl_Size;
# define Tcl_GetSizeIntFromObj Tcl_GetIntFromObj
# define Tcl_NewSizeIntObj Tcl_NewIntObj
# define TCL_SIZE_MAX      INT_MAX
# define TCL_SIZE_MODIFIER ""
#endif


/* Extension data structures */

/* Function prototypes */

#ifdef __cplusplus
extern "C" {
#endif

/* Add other command implementing functions here. */
Tcl_ObjCmdProc MyCmdObjCmd;
Tcl_ObjCmdProc BuildInfoObjCmd;

DLLEXPORT int Utf8proc_Init(Tcl_Interp* interp);

#ifdef __cplusplus
}
#endif

#endif /* _TCLUTF8PROC_H */
