/** @file nsconfig.h
*/
//--------------------------------------------
#ifndef __NSCONFIG_H__
#define __NSCONFIG_H__
//--------------------------------------------
#include <cc++/config.h>
#undef PACKAGE  //避免與 cc++ 的定義衝突
#undef PACKAGE_BUGREPORT
#undef PACKAGE_NAME
#undef PACKAGE_STRING
#undef PACKAGE_TARNAME
#undef PACKAGE_VERSION
#undef VERSION
//--------------------------------------------
#ifdef WIN32//defined in <cc++/config.h>
//#include "nsconfig_win.h"
#else
#  include "nsconfig_unix.h"
#  if !defined(HAVE_NETIPX_IPX_H)
#    define  NS_NO_IPX
#  endif
#endif
//--------------------------------------------
#endif
