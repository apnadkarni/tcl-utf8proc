# Tcl utf8proc package

The `utf8proc` Tcl package provides bindings to the
[utf8proc](https://juliastrings.github.io/utf8proc/) C library that
implements various Unicode operations on UTF-8 encoded data.

The package supports Tcl 9. There is no plan to support Tcl 8.

## Usage

```
package require utf8proc
utf8proc::normalize ?-mode MODE? ?-profile PROFILE? STRING
utf8proc::build-info ?commit|compiler|version|patchlevel?
utf8proc::pkgconfig ?list | get KEY?
utf8proc::unicodeversion
```

### normalize
The `normalize` command converts the passed string to normalization form
specified by the `-mode` option. `MODE` must be one of `NFC`, `NFD`, `NFKC`
or `NFKD` as specified by the Unicode standard and defaults to `NFC`. The
`-profile` option has the same semantics as that in Tcl's `encoding` command
but will only accept `strict` (default) and `replace` as valid values.

### build-info
The `build-info` command returns build information for the extension in
the same form as the Tcl `tcl::build-info` command.

### pkginfo
The `pkginfo` command is analogous to the `tcl::pkginfo` command. Supported
keys are `version` which returns the package version and `libversion` which
return the version of the underlying `utf8proc` C library.

### unicodeversion
The `unicodeversion` command returns the version of the Unicode standard
implemented by the library.

## Building

The extension is built as per the Tcl Extension Architecture.

### Building with autotools

If using `autotools`, and building against the system-installed Tcl,

```
mkdir build
cd build
../configure
make
make install
```

If not building against the system Tcl, replace the `configure` line with

```
../configure --with-tcl=/path/to/tcl/lib --with-tclinclude=/path/to/tcl/include
```

Add `--prefix` if you want to install somewhere other than the system default.

### Building with nmake

If building with `nmake` and Visual C++,

```
cd win
nmake /f makefile.vc /nologo INSTALLDIR=c:/path/to/tcl
nmake /f makefile.vc /nologo INSTALLDIR=c:/path/to/tcl install
```




