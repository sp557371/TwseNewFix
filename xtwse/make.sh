mkdir config
/usr/bin/autoheader
libtoolize --force --copy
#aclocal -I m4
aclocal 
automake --foreign --copy --add-missing
autoconf
./configure --prefix=/home/dev/xtwse/kwaylib --with-gnu-ld 
#./configure --prefix=/home/rd/xtwse/kwaylib --with-gnu-as --with-gnu-ld 
#./configure --prefix=$HOME/xtwse/kwaylib --build=powerpc-ibm-aix5.3.0.0 --host=powerpc-ibm-aix5.3.0.0 --target=powerpc-ibm-aix5.3.0.0 LDFLAGS=-Wl,-brtl,-bnoipath


