// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__B05A63F4_9900_4362_9E2A_2C8E360B3B70__INCLUDED_)
#define AFX_STDAFX_H__B05A63F4_9900_4362_9E2A_2C8E360B3B70__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined(_MSC_VER)
   #pragma warning( disable : 4503 4786 )
#endif

#define WIN32_LEAN_AND_MEAN

#include <stdio.h>
#include "../config_windows.h"

#if defined(_STLP_MSVC)
  #define HAVE_STLPORT 1
#endif

#if _MSC_VER >= 1300
  #define TERMINATE_IN_STD 1
#endif

#define _WIN32_DCOM

#if defined(__BORLANDC__)
   #error "�Ф� include stdafx.h"
#endif

#include "KConfig.h"

#import <msxml3.dll> raw_interfaces_only named_guids
using namespace MSXML2;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__B05A63F4_9900_4362_9E2A_2C8E360B3B70__INCLUDED_)
