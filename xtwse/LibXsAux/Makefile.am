#============================================================================
# Makefile.am for libnsAux projects
#============================================================================
ACLOCAL_AMFLAGS = -I m4

INCLUDES = -I$(includedir)/cc++2 \
           -I/usr/local/include/cc++2 \
           -I/usr/include/cc++2 \
           -I../LibXS \
           -I../LibXS/Signon

DEFS = -DHAVE_CONFIG_H 

lib_LTLIBRARIES = libxsAux.la

libxsAux_la_LDFLAGS = -version-info 0:0:0

libxsAux_la_LIBADD  = -L$(libdir) -lccgnu2 $(LDL)\
                      -lxs

libxsAux_la_SOURCES = Nsa_SysInfo.cpp\
                      Nsa_SeqNoGene.cpp\
                      Nsa_TextSimpReco.cpp\
                      Nsa_Time.cpp\
                      Nsa_LogFile.cpp\
                      Nsa_UtilComm.cpp\
                      Nsa_UtilStr.cpp\
                      Nsa_UtilTime.cpp\
                      Nsa_LineBuf.cpp\
                      Nsa_GlobObjs.cpp\
                      Nsa_FileWriter.cpp\
                      Nsa_DbgInfo.cpp\
                      Nsa_UtilFile.cpp\
                      Nsa_EvReplay.cpp\
                      Nsa_SimpThread.cpp
