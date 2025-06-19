# Release process

A manual process that should be automated. Some day ...

Assumes release as in working directory ...

## Preliminaries

Update Docs

- Update README.md
- Commit
- Tag release
- Push to github
- Push tag to github
- Verify CI builds pass

## Windows binaries

Run release.cmd to generate Windows binary distribution under `dist/utf8proc-VERSION`.
Alternative, run release-mingw.cmd to generate Windows binaries using MinGW
instead. The binaries will be under dist/mingw/utf8proc-VERSION.
This will build the binaries for Tcl 8.6 and 9 for both x86 and x64.

Unzip into distribution into each of the 8.6 and 9.0 directories for
x86 and x64. Test that the package loads successfully.

## Source code archives

One of the following, both from WSL.

### Method 1

```
PKG=tcl-utf8proc
PKGVER=$(egrep AC_INIT configure.ac | egrep -o --color=never '[0-9]\.[0-9][ab.][0-9]')
/mnt/c/bin/git-archive-all.sh --tree-ish v$PKGVER --prefix ${PKG}$PKGVER/ --format tar --verbose /tmp/${PKG}$PKGVER-src.tar
cd /tmp
tar xf /tmp/${PKG}$PKGVER-src.tar
gzip ${PKG}$PKGVER-src.tar
zip -r -q ${PKG}$PKGVER-src.zip ${PKG}$PKGVER
```

NOTE: only use tar format above. Directly specifying zip or tgz creates
separate submodule archives.

### Method 2

```
cd /tmp
git clone --recurse-submodules git@github.com:apnadkarni/tcl-${PKG}.git ${PKG}-u
cd ${PKG}-u
PKGVER=$(egrep AC_INIT configure.ac | egrep -o --color=never '[0-9]\.[0-9][ab.][0-9]')
git archive --prefix ${PKG}$PKGVER-src/ v$PKGVER | (cd /tmp ; tar xf -)
cd ./tclh
git archive --prefix ${PKG}$PKGVER-src/tclh/ HEAD | (cd /tmp ; tar xf -)
cd /tmp
tar -czf ${PKG}$PKGVER-src.tgz ${PKG}$PKGVER-src
zip -r ${PKG}$PKGVER-src.zip ${PKG}$PKGVER-src
```

Extract the archives somewhere and ensure it builds.

## Web site documentation

Upload documentation.
