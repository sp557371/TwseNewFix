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

rm01="make clean"

while read mpath
do
   if [ -n "$mpath" ]
   then
      cd $HOME
      cd $mpath

      $rm01
   fi

done < mklist.all
echo
echo
echo "make all clean!!"
echo
echo
