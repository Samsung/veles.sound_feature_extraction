#!/bin/sh

if [ ! -e "fftf" ]; then
	git submodule update --init	
else
    git submodule update
fi

if [ ! -e "fftf/configure" ]; then
	cd fftf
	fftf/autogen.sh
	cd ..
fi

isubuntu="$(uname -v|grep Ubuntu)"

check_prog() {
    printf "Checking for $1... "
    if [ -z "$($1 --version 2>/dev/null)" ]; then
        echo no
        if [ -n "$isubuntu" ]; then
            sudo apt-get -y install $2
            printf "Checking for $1... "
            if [ -z "$($1 --version 2>/dev/null)" ]; then
                echo no
                echo "Error: $1 was not found, aborting"
                exit 1
            fi
        else
            echo "Error: $1 was not found, aborting"
            exit 1
        fi
    fi
    echo yes
}

check_prog aclocal automake
check_prog autoheader autoconf
check_prog autoconf autoconf
check_prog libtoolize libtool
check_prog automake automake

rm -rf autom4te.cache m4
rm -f aclocal.m4 ltmain.sh config.log config.status configure libtool stamp-h1 config.h config.h.in
find -name Makefile.in -exec rm {} \;

echo "Running aclocal..." ; aclocal $ACLOCAL_FLAGS || exit 1
echo "Running autoheader..." ; autoheader || exit 1
echo "Running autoconf..." ; autoconf || exit 1
echo "Running libtoolize..." ; (libtoolize --copy --automake || glibtoolize --automake) || exit 1
echo "Running automake..." ; (automake --add-missing --copy --foreign ) || exit 1

W=0

rm -f config.cache-env.tmp
echo "OLD_PARM=\"$@\"" >> config.cache-env.tmp
echo "OLD_CFLAGS=\"$CFLAGS\"" >> config.cache-env.tmp
echo "OLD_PATH=\"$PATH\"" >> config.cache-env.tmp
echo "OLD_PKG_CONFIG_PATH=\"$PKG_CONFIG_PATH\"" >> config.cache-env.tmp
echo "OLD_LDFLAGS=\"$LDFLAGS\"" >> config.cache-env.tmp

cmp -s config.cache-env.tmp config.cache-env >> /dev/null
if [ $? -ne 0 ]; then
	W=1;
fi

if [ $W -ne 0 ]; then
	echo "Cleaning configure cache...";
	rm -f config.cache config.cache-env
	mv config.cache-env.tmp config.cache-env
else
	rm -f config.cache-env.tmp
fi
