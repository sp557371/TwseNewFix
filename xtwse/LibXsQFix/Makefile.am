#============================================================================
# libxsQFix
#============================================================================
INCLUDES= -I$(includedir)/cc++2\
           -I/usr/include/libxml2\
           -I../LibXS\
           -I../LibXsAux\
           -I.

DEFS = -DHAVE_CONFIG_H

lib_LTLIBRARIES = libxsQFix.la libxsQFixExt.la

#============================================================================
# libxsQFix
#============================================================================
libxsQFix_la_LDFLAGS	= -version-info 0:0:0 

libxsQFix_la_LIBADD	= -L. -L/usr/lib -lxml2 -L$(libdir) -lccgnu2  -lxs  

libxsQFix_la_SOURCES = src/Code/CallStack.cpp\
                       src/Code/DataDictionary.cpp\
                       src/Code/FieldMap.cpp\
                       src/Code/FieldTypes.cpp\
                       src/Code/Group.cpp\
                       src/Code/Message.cpp\
                       src/Code/LIBXML_DOMDocument.cpp\
                       src/Code/UtilityQ.cpp\
                       src/Code/MessageSorters.cpp\
                       src/Code/FieldConvertors.cpp\
                       src/Code/Field.cpp


#============================================================================
# libxsQFixExt
#============================================================================
libxsQFixExt_la_LDFLAGS	= -version-info 0:0:0

libxsQFixExt_la_LIBADD	= -L. -L$(libdir) -lccgnu2 -lxsQFix

libxsQFixExt_la_SOURCES = ./QFixExt/QFixField.cpp\
                          ./QFixExt/QFixTagTypeBase.cpp\
                          ./QFixExt/QFixTagType.cpp\
                          ./QFixExt/QFixType.cpp\
                          ./QFixExt/QFixTagDef.cpp\
                          ./QFixExt/QFixUtil.cpp
