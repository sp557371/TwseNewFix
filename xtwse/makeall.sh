if [ -z "$1" ]
then
   echo "usage: $0  para1  para2"
   echo "para1 = [ all | ft ]"
   echo "para2 = [ new | clean |    ] "
   exit 1;
fi

if [ ! -r "mklist.$1" ]
then 
   echo "***************************************"
   echo
   echo " ERROR!!!"
   echo " mklist.$1 not exist, please check!"
   echo
   echo "***************************************"
   exit 1;
fi

cd kwaylib/lib
ls *.so.0.0.0 > ../liblist


while read libname
do
   libsname=`echo $libname|awk '{split($0,arr,".");print arr[1]}'`
   libso=$libsname.so
   if [ -f $libso ]
   then
      echo "$libso already exist, do nothing!"
   else
      echo "$libso not exist, auto create!"
      ln -s $libname $libso
   fi
   libso=$libsname.so.0
   if [ -f $libso ]
   then
      echo "$libso already exist, do nothing!"
   else
      echo "$libso not exist, auto create!"
      ln -s $libname $libso
   fi;
      
done < ../liblist
cd ../..

rm01="rm -rf .deps"
rm02="rm -rf .libs"
rm03="rm -rf autom4te.cache"
rm04="rm -rf config"
rm05="rm -rf configure"
rm06="rm -rf Makefile"
rm07="rm -rf libtool"
rm08="rm -rf *.in*"
rm09="rm -rf *.la"
rm10="rm -rf config.log"
rm11="rm -rf aclocal.m4"
rm12="rm -rf config.status"
rm13="rm -rf autoscan.log"
rm14="rm -rf stamp-h1"
rm15="rm -rf config.h"
rm16="rm -rf *.lo"

mk1="mkdir config"
mk2="/usr/bin/autoheader"
mk3="libtoolize --force --copy"
mk4="aclocal"
mk5="automake --foreign --copy --add-missing"
mk6="autoconf"
#mk7="./configure --prefix=$HOME/xtwse/kwaylib --build=powerpc-ibm-aix5.3.0.0 --host=powerpc-ibm-aix5.3.0.0 --target=powerpc-ibm-aix5.3.0.0 LDFLAGS=-Wl,-brtl,-bnoipath"
mk7="./configure --prefix=/$HOME/xtwse/kwaylib --with-gnu-ld"

mk8="make install"



while read mpath
do
   if [ -n "$mpath" ]
   then
      cd $HOME
      cd $mpath

      case $2 in
      'new')
            $rm01;$rm02;$rm03;$rm04;$rm05;$rm06;$rm07;$rm08;$rm09;$rm10;$rm11;$rm12;$rm13;$rm14;$rm15;$rm16
            if [ ! -d "config" ]
            then
               $mk1;
            fi
            $mk2;$mk3;$mk4;$mk5;$mk6;$mk7
            make clean;
         ;;
      'clean')
            if [ ! -r "Makefile" ]
            then
               $rm01;$rm02;$rm03;$rm04;$rm05;$rm06;$rm07;$rm08;$rm09;$rm10;$rm11;$rm12;$rm13;$rm14;$rm15;$rm16
               if [ ! -d "config" ]
               then
                  $mk1;
               fi
               $mk2;$mk3;$mk4;$mk5;$mk6;$mk7;
            fi
            make clean;
         ;;
      *)
           if [ ! -r "Makefile" ]
            then
               $rm01;$rm02;$rm03;$rm04;$rm05;$rm06;$rm07;$rm08;$rm09;$rm10;$rm11;$rm12;$rm13;$rm14;$rm15;$rm16
               if [ ! -d "config" ]
               then
                  $mk1;
               fi
               $mk2;$mk3;$mk4;$mk5;$mk6;$mk7
               make clean;
            fi
         ;;
      esac  
       
      if $mk8 
      then
         echo
         echo
         echo "`pwd` make success!"
         echo
         echo;
      else
         echo
         echo
         echo "`pwd` error and exit!"
         echo
         echo
         exit 1;
      fi ;
   fi

done < mklist.$1
echo
echo
echo "make all finish!!"
echo
echo
