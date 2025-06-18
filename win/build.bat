cd %~dp0
IF NOT DEFINED VSCMD_ARG_TGT_ARCH call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
nmake /s /f makefile.vc INSTALLDIR=C:\Tcl\9.0.0\%VSCMD_ARG_TGT_ARCH% OPTS=pdbs cdebug="-Zi -Od" %*

