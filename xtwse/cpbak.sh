
rm XTwseGroup.tar.gz

#============================================================================
# XTwseC
#============================================================================
tar cvf XTwseGroup.tar `find ./XTwseC -name "*.cpp"`
tar rvf XTwseGroup.tar `find ./XTwseC -name "*.h"`
tar rvf XTwseGroup.tar `find ./XTwseC -name "Makefile.am"`
tar rvf XTwseGroup.tar `find ./XTwseC -name "cp*.sh"`
#============================================================================
# XTester
#============================================================================
tar rvf XTwseGroup.tar `find ./XTester -name "*.cpp"`
tar rvf XTwseGroup.tar `find ./XTester -name "*.h"`
tar rvf XTwseGroup.tar `find ./XTester -name "Makefile.am"`
tar rvf XTwseGroup.tar `find ./XTester -name "cp*.sh"`
#============================================================================
# XCtrl
#============================================================================
tar rvf XTwseGroup.tar `find ./XCtrl -name "*.cpp"`
tar rvf XTwseGroup.tar `find ./XCtrl -name "*.h"`
tar rvf XTwseGroup.tar `find ./XCtrl -name "Makefile.am"`
tar rvf XTwseGroup.tar `find ./XCtrl -name "cp*.sh"`
#============================================================================
# libxsAux
#============================================================================
tar rvf XTwseGroup.tar `find ./LibXsAux -name "*.cpp"`
tar rvf XTwseGroup.tar `find ./LibXsAux -name "*.cc"`
tar rvf XTwseGroup.tar `find ./LibXsAux -name "*.h"`
tar rvf XTwseGroup.tar `find ./LibXsAux -name "Makefile.am"`
#============================================================================
# libxsQFix
#============================================================================
tar rvf XTwseGroup.tar `find ./LibXsQFix -name "*.c"`
tar rvf XTwseGroup.tar `find ./LibXsQFix -name "*.cpp"`
tar rvf XTwseGroup.tar `find ./LibXsQFix -name "*.cc"`
tar rvf XTwseGroup.tar `find ./LibXsQFix -name "*.h"`
tar rvf XTwseGroup.tar `find ./LibXsQFix -name "*.tlh"`
tar rvf XTwseGroup.tar `find ./LibXsQFix -name "Makefile.am"`
#============================================================================
# libxsSmAppia
#============================================================================
tar rvf XTwseGroup.tar `find ./LibXsSmAppia -name "*.cpp"`
tar rvf XTwseGroup.tar `find ./LibXsSmAppia -name "*.h"`
tar rvf XTwseGroup.tar `find ./LibXsSmAppia -name "Makefile.am"`
#============================================================================
# libxsFixStk
#============================================================================
tar rvf XTwseGroup.tar `find ./LibXsFixStk -name "*.cpp"`
tar rvf XTwseGroup.tar `find ./LibXsFixStk -name "*.h"`
tar rvf XTwseGroup.tar `find ./LibXsFixStk -name "Makefile.am"`
#============================================================================
# libxsTwse
#============================================================================
tar rvf XTwseGroup.tar `find ./LibXsTwse -name "*.cpp"`
tar rvf XTwseGroup.tar `find ./LibXsTwse -name "*.h"`
tar rvf XTwseGroup.tar `find ./LibXsTwse -name "Makefile.am"`
#============================================================================
# libxs
#============================================================================
tar rvf XTwseGroup.tar `find ./LibXS -name "*.c"`
tar rvf XTwseGroup.tar `find ./LibXS -name "*.cpp"`
tar rvf XTwseGroup.tar `find ./LibXS -name "*.cc"`
tar rvf XTwseGroup.tar `find ./LibXS -name "*.h"`
tar rvf XTwseGroup.tar `find ./LibXS -name "*.hpp"`
tar rvf XTwseGroup.tar `find ./LibXS -name "Makefile.am"`
#============================================================================
# LibExt
#============================================================================
tar rvf XTwseGroup.tar `find ./LibExt -name "*.cpp"`
tar rvf XTwseGroup.tar `find ./LibExt -name "*.h"`
tar rvf XTwseGroup.tar `find ./LibExt -name "Makefile.am"`
#============================================================================
# LibStkExt
#============================================================================
tar rvf XTwseGroup.tar `find ./LibStkExt -name "*.cpp"`
tar rvf XTwseGroup.tar `find ./LibStkExt -name "*.h"`
tar rvf XTwseGroup.tar `find ./LibStkExt -name "Makefile.am"`
#============================================================================
# LibTMPSes
#============================================================================
tar rvf XTwseGroup.tar `find ./LibTMPSes -name "*.cpp"`
tar rvf XTwseGroup.tar `find ./LibTMPSes -name "*.h"`
tar rvf XTwseGroup.tar `find ./LibTMPSes -name "Makefile.am"`
#============================================================================
# FT
#============================================================================
tar rvf XTwseGroup.tar `find ./FT -name "*.cpp"`
tar rvf XTwseGroup.tar `find ./FT -name "*.h"`
tar rvf XTwseGroup.tar `find ./FT -name "Makefile.am"`
#============================================================================
# TestTools
#============================================================================
tar rvf XTwseGroup.tar `find ./TestTools -name "*.cpp"`
tar rvf XTwseGroup.tar `find ./TestTools -name "*.h"`
tar rvf XTwseGroup.tar `find ./TestTools -name "Makefile.am"`
#============================================================================
# Other
#============================================================================
tar rvf XTwseGroup.tar `find ./ -name "cpbak.sh"`

gzip XTwseGroup.tar
