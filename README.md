# Tcl utf8proc package

The `utf8proc` Tcl package provides bindings to the
[utf8proc](https://juliastrings.github.io/utf8proc/) C library that
implements various Unicode operations on UTF-8 encoded data.

The package supports Tcl 9. There is no plan to support Tcl 8.

See file LICENSE for licensing terms.

## Usage

```
package require utf8proc
utf8proc::build-info ?commit|compiler|version|patchlevel?
utf8proc::categorize STRING
utf8proc::normalize ?-mode MODE? ?-profile PROFILE? STRING
utf8proc::pkgconfig ?list | get KEY?
utf8proc::unicodeversion
```

### build-info
The `build-info` command returns build information for the extension in
the same form as the Tcl `tcl::build-info` command.

### categorize

Returns a list of categories, each element of which is a category code for
each letter in the passed string. The category codes may be one of
"Cn","Lu","Ll","Lt","Lm","Lo","Mn","Mc","Me","Nd","Nl","No","Pc","Pd","Ps",
"Pe","Pi","Pf","Po","Sm","Sc","Sk","So","Zs","Zl","Zp","Cc","Cf","Cs" or "Co"
as defined in [Table 4-4](https://www.unicode.org/versions/Unicode16.0.0/core-spec/chapter-4/#G134153)
of the Unicode Standard or the string `??` for code points that are invalid.

### normalize
The `normalize` command converts the passed string to normalization form
specified by the `-mode` option.

`MODE` must be one of `nfc`, `nfd`, `nfkc`, `nfkd` or `nfkccasefold` and
defaults to `NFC`. See [Unicode normalization forms](https://www.unicode.org/versions/Unicode16.0.0/core-spec/chapter-3/#G49537)
and [Unicode case folding](https://www.unicode.org/versions/Unicode16.0.0/core-spec/chapter-3/#G53288).

The
`-profile` option has the same semantics as that in Tcl's `encoding` command
but will only accept `strict` (default) and `replace` as valid values.

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




