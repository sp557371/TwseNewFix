#============================================================================
# 2004/02/03..07/13
# Makefile.am : create Shared library libFTdsalModule (by GNU's libtool)
# Ref: www.gnu.org
#============================================================================
AM_CPPFLAGS     = -I../../../../LibXS\
		                -I../../../../LibXS/tinyxml\
              		  -I../../../../LibExt\
              		  -I../../FTCenter\
              		  -I../../base\
              		  -I../../bxLink\
               	  -I../../bxFT\
                  -I../../bxSes\
                  -I../../Notify\
                  -I../../LibFTrm\
                  -I../../bxDR\
                  -I../../../../LibXsTwse\
                  -I../../../../LibTMPSes\
              		  -I../
lib_LTLIBRARIES = libFTTwse.la
#============================================================================
# Create Library : libFTdsal.la + Other Library
# Other  Library : libns.la
#============================================================================
libFTTwse_la_LDFLAGS = -version-info 0:0:0 
libFTTwse_la_LIBADD  = -L. -L$(libdir) -lxs -lExt -lFTExt -lxsTwse
libFTTwse_la_SOURCES = Twse.cpp\
	       	       TwseSets.cpp\
                       Twse2KwayFiles.cpp\
                       TwseFilesHandler.cpp\
                       GFiledDef.h\
                       TwseFileLog.cpp\
                       ../FTExtDef.cpp\
                       ../../../../LibXsTwse/TwseType.h\
                       ../../../../LibXS/tinyxml/tinystr.cpp\
                       ../../../../LibXS/tinyxml/tinyxml.cpp\
                       ../../../../LibXS/tinyxml/tinyxmlerror.cpp\
                       ../../../../LibXS/tinyxml/tinyxmlparser.cpp

