/*
 * tclUtf8Proc.c --
 *
 * Main file for the utf8proc Tcl extension.
 *
 * See the file LICENSE for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 */

#include "tclUtf8Proc.h"
#include <string.h>
#include <assert.h>

#define STRINGIZEx(x) #x
#define STRINGIZE(x) STRINGIZEx(x)
#define UTF8PROCLIBVERSION \
  STRINGIZE(UTF8PROC_VERSION_MAJOR) "." STRINGIZE(UTF8PROC_VERSION_MINOR) "." STRINGIZE(UTF8PROC_VERSION_PATCH)

/* A way to mark a code path as unreachable - from tclInt.h */
#ifndef TCL_UNREACHABLE
#if defined(__STDC__) && __STDC__ >= 202311L
#include <stddef.h>
#define TCL_UNREACHABLE()	unreachable()
#elif defined(__GNUC__)
#define TCL_UNREACHABLE()	__builtin_unreachable()
#elif defined(_MSC_VER)
#include <stdbool.h>
#define TCL_UNREACHABLE()	__assume(false)
#else
#define TCL_UNREACHABLE()	((void) 0)
#endif
#endif // TCL_UNREACHABLE

/*
 * Metainfo for a pkgconfig command for the extension via Tcl_RegisterConfig
 * Must only have const static UTF-8 encoded char pointers.
 */
Tcl_Config utf8procConfig[] = {
    {"version", PACKAGE_VERSION},
    {"libversion", UTF8PROCLIBVERSION},
    /* Add additional configuration or feature information if relevant */
    {NULL, NULL}
};

/*
 * Tcl_UnicodeVersionObjCmd --
 *
 *	 Returns the Unicode version supported by utf8proc.
 *
 * Results:
 *	A standard Tcl result
 *
 * Side effects:
 *	None.
 */

static int
Tcl_UnicodeVersionObjCmd(
    void *dummy,		/* Not used. */
    Tcl_Interp *interp,		/* Current interpreter */
    int objc,			/* Number of arguments */
    Tcl_Obj *const objv[]	/* Argument strings */
    )
{
    Tcl_SetObjResult(interp, Tcl_NewStringObj(utf8proc_unicode_version(), -1));
    return TCL_OK;
}

/*
 * Tcl_UnicodeNormalizeObjCmd --
 *
 *	 Implements the "normalize" command for Unicode normalization.
 *
 * Results:
 *	A standard Tcl result
 *
 * Side effects:
 *	Sets the interpreter result to the normalized string.
 */

static int
Tcl_UnicodeNormalizeObjCmd(
    void *dummy,		/* Not used. */
    Tcl_Interp *interp,		/* Current interpreter */
    int objc,			/* Number of arguments */
    Tcl_Obj *const objv[]	/* Argument strings */
    )
{
    static const char *normalizationForms[] = {
        "nfc", "nfd", "nfkc", "nfkd", NULL};
    typedef enum { MODE_NFC, MODE_NFD, MODE_NFKC, MODE_NFKD } NormalizationMode;
    static const char *optNames[] = {"-profile", "-mode", NULL};
    enum { OPT_PROFILE, OPT_MODE } opt;

    if (objc < 2) {
	Tcl_WrongNumArgs(interp, 1, objv, "?-profile PROFILE? ?-mode MODE? STRING");
	return TCL_ERROR;
    }

    int i;
    int profile = TCL_ENCODING_PROFILE_STRICT;
    NormalizationMode mode = MODE_NFC;
    for (i = 1; i < objc - 1; ++i) {
	if (Tcl_GetIndexFromObj(
		interp, objv[i], optNames, "option", 0, &opt) != TCL_OK) {
	    return TCL_ERROR;
	}
	++i;
	if (i == (objc-1)) {
	    Tcl_SetObjResult(interp,
			     Tcl_ObjPrintf("Missing value for option %s.",
					   Tcl_GetString(objv[i - 1])));
	    return TCL_ERROR;
	}
	const char *s = Tcl_GetString(objv[i]);
	switch (opt) {
	case OPT_PROFILE:
	    if (!strcmp(s, "replace")) {
		profile = TCL_ENCODING_PROFILE_REPLACE;
	    } else if (!strcmp(s, "strict")) {
		profile = TCL_ENCODING_PROFILE_STRICT;
            } else {
                Tcl_SetObjResult(interp,
		    Tcl_ObjPrintf("Invalid value \"%s\" supplied for option \"-profile\". Must be "
				  "\"strict\" or \"replace\".",
				  s));
		return TCL_ERROR;
            }
            break;
	case OPT_MODE:
	    if (Tcl_GetIndexFromObj(interp, objv[i], normalizationForms, "normalization mode", 0, &mode) != TCL_OK) {
		return TCL_ERROR;
	    }
	    break;
	default:
	    TCL_UNREACHABLE();
	}
    }

    Tcl_Encoding encoding = Tcl_GetEncoding(interp, "utf-8");
    if (encoding == NULL) {
        return TCL_ERROR;
    }

    int result;
    Tcl_DString ds;
    result = Tcl_UtfToExternalDStringEx(interp, encoding,
                Tcl_GetString(objv[objc - 1]), -1, profile, &ds, NULL);
    /* !!! dsIn needs to be freed even in case of error returns */

    if (result != TCL_OK) {
        result = TCL_ERROR; /* Translate TCL_CONVERT_* errors to TCL_ERROR */
    } else {
	Tcl_Size dsLength = Tcl_DStringLength(&ds);
        const utf8proc_uint8_t *dsStr = (utf8proc_uint8_t *) Tcl_DStringValue(&ds);
        const utf8proc_uint8_t *normalizedUtf8;
        utf8proc_ssize_t normalizedLength;
        switch (mode) {
        case MODE_NFC:
            normalizedLength = utf8proc_map_custom(
                dsStr, dsLength, &normalizedUtf8, UTF8PROC_STABLE|UTF8PROC_COMPOSE, NULL, NULL);
            break;
        case MODE_NFD:
            normalizedLength = utf8proc_map_custom(
                dsStr, dsLength, &normalizedUtf8, UTF8PROC_STABLE|UTF8PROC_DECOMPOSE, NULL, NULL);
            break;
        case MODE_NFKC:
            normalizedLength = utf8proc_map_custom(
                dsStr, dsLength, &normalizedUtf8, UTF8PROC_STABLE|UTF8PROC_COMPOSE|UTF8PROC_COMPAT, NULL, NULL);
            break;
        case MODE_NFKD:
            normalizedLength = utf8proc_map_custom(
                dsStr, dsLength, &normalizedUtf8, UTF8PROC_STABLE|UTF8PROC_DECOMPOSE|UTF8PROC_COMPAT, NULL, NULL);
            break;
        }
	if (normalizedLength < 0) {
            const char *errorMsg = utf8proc_errmsg(normalizedLength);
            Tcl_SetObjResult(
                interp, Tcl_NewStringObj(
                    errorMsg ? errorMsg : "Unicode normalization failed.", -1));
            result = TCL_ERROR;
        } else {
            /* Convert standard UTF8 to internal UTF8 */
            assert(normalizedUtf8);
            Tcl_DStringSetLength(&ds, 0);
            result = Tcl_ExternalToUtfDStringEx(interp, encoding,
                (const char *)normalizedUtf8, normalizedLength, profile, &ds, NULL);
            free(normalizedUtf8);
	    if (result == TCL_OK) {
                Tcl_DStringResult(interp, &ds);
            }
        }
    }

    Tcl_DStringFree(&ds);
    Tcl_FreeEncoding(encoding);
    /* Translate TCL_CONVERT* etc. codes to TCL_ERROR */
    return result == TCL_OK ? TCL_OK : TCL_ERROR;
}

/*
 * Tcl_UnicodeCategorizeObjCmd --
 *
 *	 Implements the "categorize" command.
 *
 * Results:
 *	A standard Tcl result
 *
 * Side effects:
 *	Sets the interpreter result to a list of Unicode categories.
 */

static int
Tcl_UnicodeCategorizeObjCmd(
    void *dummy,		/* Not used. */
    Tcl_Interp *interp,		/* Current interpreter */
    int objc,			/* Number of arguments */
    Tcl_Obj *const objv[]	/* Argument strings */
    )
{
    if (objc != 2) {
        Tcl_WrongNumArgs(interp, 1, objv, "STRING");
        return TCL_ERROR;
    }

    Tcl_Obj *categoryObjs[31]; /* 30 categories + one for out of range */
    for (int i = 0; i < sizeof(categoryObjs)/sizeof(categoryObjs[0]); ++i) {
        categoryObjs[i] = NULL;
    }

    Tcl_Obj *srcObj = objv[1];
    Tcl_Size len = Tcl_GetCharLength(srcObj);
    Tcl_Obj *resultList = Tcl_NewListObj(len, NULL);
    for (int i = 0; i < len; ++i) {
        Tcl_UniChar uc = Tcl_GetUniChar(srcObj, i);
        utf8proc_category_t category;
        if (uc < 0 || uc > 0x10FFFF) {
            category = 30;
            if (categoryObjs[30] == NULL) {
                categoryObjs[30] = Tcl_NewStringObj("??", -1);
            }
        } else {
            category = utf8proc_category(uc);
            if (categoryObjs[category] == NULL) {
                categoryObjs[category] =
                    Tcl_NewStringObj(utf8proc_category_string(category), -1);
            }
        }
        Tcl_ListObjAppendElement(interp, resultList, categoryObjs[category]);
    }
    for (int i = 0; i < sizeof(categoryObjs)/sizeof(categoryObjs[0]); ++i) {
        if (categoryObjs[i] != NULL) {
	    Tcl_BounceRefCount(categoryObjs[i]);
	}
    }
    Tcl_SetObjResult(interp, resultList);
    return TCL_OK;
}

/*
 * Tcl_UnicodeMakeRawObjCmd --
 *
 *	Implements the "makerawobj" command. USE ONLY FOR TESTING. Exists
 *	to check behaviour on invalid data passed in via C API.
 *
 * Results:
 *	A standard Tcl result
 *
 * Side effects:
 *	Sets the interpreter result to a Tcl object containing raw data.
 */
static int
Tcl_UnicodeMakeRawObjCmd(
    void *dummy,		/* Not used. */
    Tcl_Interp *interp,		/* Current interpreter */
    int objc,			/* Number of arguments */
    Tcl_Obj *const objv[]	/* Argument strings */
    )
{
    if (objc != 3) {
        Tcl_WrongNumArgs(interp, 1, objv, "string|unistring BINARY");
        return TCL_ERROR;
    }

    Tcl_Size numBytes;
    unsigned char *bytes = Tcl_GetBytesFromObj(interp, objv[2], &numBytes);
    if (bytes == NULL) {
	return TCL_ERROR;
    }
    const char *opt = Tcl_GetString(objv[1]);
    Tcl_Obj *resultObj;
    if (!strcmp(opt, "string")) {
        resultObj = Tcl_NewStringObj((const char *)bytes, numBytes);
    } else if (!strcmp(opt, "unistring")) {
	if (numBytes % sizeof(Tcl_UniChar) != 0) {
            Tcl_SetObjResult(
                interp,
                Tcl_ObjPrintf("Invalid byte length %" TCL_SIZE_MODIFIER "u"
                              " for unistring. Must be "
                              "a multiple of %" TCL_SIZE_MODIFIER "u.",
                              numBytes,
                              sizeof(Tcl_UniChar)));
							     return TCL_ERROR;
	}
        resultObj = Tcl_NewUnicodeObj((const Tcl_UniChar *)bytes,
                                         numBytes / sizeof(Tcl_UniChar));
    } else {
        Tcl_SetObjResult(
            interp,
            Tcl_ObjPrintf(
                "Unknown option \"%s\". Must be \"string\" or \"unistring\".",
                opt));
        return TCL_ERROR;
    }
    Tcl_SetObjResult(interp, resultObj);
    return TCL_OK;
}

/*
 * Utf8proc_Init --
 *
 *	Initialize the extension etc.
 *
 * Results:
 *	A standard Tcl result
 *
 * Side effects:
 *	The package is registered with the interpreter.
 *	Commands ... are added to the interpreter.
 */

DLLEXPORT int
Utf8proc_Init(
    Tcl_Interp* interp)		/* Tcl interpreter */
{
    /*
     * Support any Tcl version compatible with the version against which the
     * extension is being built.
     */
    if (Tcl_InitStubs(interp, TCL_VERSION, 0) == NULL) {
	return TCL_ERROR;
    }

    /*
     * Do any required package initialization.
     */

    /*
     * Register the commands added by the package.
     */
    Tcl_CreateObjCommand(interp, PACKAGE_NAME "::" "unicodeversion", Tcl_UnicodeVersionObjCmd, NULL, NULL);
    Tcl_CreateObjCommand(interp, PACKAGE_NAME "::" "build-info", BuildInfoObjCmd, NULL, NULL);
    Tcl_CreateObjCommand(interp, PACKAGE_NAME "::" "normalize", Tcl_UnicodeNormalizeObjCmd, NULL, NULL);
    Tcl_CreateObjCommand(interp, PACKAGE_NAME "::" "categorize", Tcl_UnicodeCategorizeObjCmd, NULL, NULL);
    Tcl_CreateObjCommand(interp, PACKAGE_NAME "::" "test::makerawobj", Tcl_UnicodeMakeRawObjCmd, NULL, NULL);


    /* Register feature configuration  */
    Tcl_RegisterConfig(interp, PACKAGE_NAME, utf8procConfig, "utf-8");


    /*
     * Inform Tcl the package is available. PACKAGE_NAME and PACKAGE_VERSION
     * are set by the build system (autoconf or nmake)
     */
    if (Tcl_PkgProvideEx(interp, PACKAGE_NAME, PACKAGE_VERSION, NULL) != TCL_OK) {
	return TCL_ERROR;
    }

    return TCL_OK;
}
