if [ ! -r "mklist.all" ]
then 
   echo "***************************************"
   echo
   echo " ERROR!!!"
   echo " mklist.all not exist, please check!"
   echo
   echo "***************************************"
   exit 1;
fi

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

while read mpath
do
   if [ -n "$mpath" ]
   then
      cd $HOME
      cd $mpath

      $rm01;$rm02;$rm03;$rm04;$rm05;$rm06;$rm07;$rm08;$rm09;$rm10;$rm11;$rm12;$rm13;$rm14;$rm15;$rm16
   fi

done < mklist.all
echo
echo
echo "rmtmp all finish!!"
echo
echo
