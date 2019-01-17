bkpath=`date '+TmpSrc_%Y%m%d'`
rm -f $bkpath.tar.gz
tar cvf $bkpath.tar `find xtwse -name "*.cpp"|grep -v LibXS`
tar rvf $bkpath.tar xtwse/LibXS/tinyxml
tar rvf $bkpath.tar xtwse/LibXS/MesTCPClientTSE.cpp
tar rvf $bkpath.tar `find xtwse -name "*.cc"`
tar rvf $bkpath.tar `find xtwse -name "*.c"`
tar rvf $bkpath.tar `find xtwse -name "*.h"`
tar rvf $bkpath.tar `find xtwse -name "*.sh"`
tar rvf $bkpath.tar `find xtwse -name "mklist*"`
tar rvf $bkpath.tar `find xtwse -name "Makefile.*"`
tar rvf $bkpath.tar `find xtwse -name configure.ac`
#tar rvf $bkpath.tar Settings
#tar rvf $bkpath.tar shell
#tar rvf $bkpath.tar xtwse/kwaylib/lib/libxs.*
#tar rvf $bkpath.tar xtwse/kwaylib/lib/libxsSignon*
#tar rvf $bkpath.tar xtwse/kwaylib/lib/libxsTwStk*
#tar rvf $bkpath.tar xtwse/kwaylib/lib/libxml2*
#tar rvf $bkpath.tar xtwse/kwaylib/lib/libcc*
#tar rvf $bkpath.tar xtwse/kwaylib/include
gzip $bkpath.tar
ls -al $bkpath.tar.gz
