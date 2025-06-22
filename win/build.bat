cd %~dp0
IF NOT DEFINED VSCMD_ARG_TGT_ARCH call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
nmake /s /f makefile.vc INSTALLDIR=C:\Tcl\main\%VSCMD_ARG_TGT_ARCH%-debug OPTS=pdbs cdebug="-Zi -Od" %*

