#!/bin/sh
# Run this to generate the configure script.

srcdir=`dirname $0`
test -z "$srcdir" && srcdir=.

DIE=0

(test -f $srcdir/configure.ac) || {
    echo -n "**Error**: Directory "\`$srcdir\'" does not look like the"
    echo " top-level package directory"
    exit 1
}

(autoconf --version) < /dev/null > /dev/null 2>&1 || {
  echo
  echo "**Error**: You must have \`autoconf' installed."
  echo "Download the appropriate package for your distribution,"
  echo "or get the source tarball at ftp://ftp.gnu.org/pub/gnu/"
  DIE=1
}


(automake --version) < /dev/null > /dev/null 2>&1 || {
  echo
  echo "**Error**: You must have \`automake' installed."
  echo "You can get it from: ftp://ftp.gnu.org/pub/gnu/"
  DIE=1
  NO_AUTOMAKE=yes
}


# if no automake, don't bother testing for aclocal
test -n "$NO_AUTOMAKE" || (aclocal --version) < /dev/null > /dev/null 2>&1 || {
  echo
  echo "**Error**: Missing \`aclocal'.  The version of \`automake'"
  echo "installed doesn't appear recent enough."
  echo "You can get automake from ftp://ftp.gnu.org/pub/gnu/"
  DIE=1
}

if test "$DIE" -eq 1; then
  exit 1
fi

libtoolize
aclocal
autoheader
autoconf
automake --add-missing

