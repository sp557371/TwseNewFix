// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//
#ifdef _MSC_VER
#pragma once

//#define _CRTDBG_MAP_ALLOC   //for detect memory leaks
//#include <stdlib.h>
//#include <crtdbg.h>

#define _CRT_SECURE_NO_DEPRECATE //fonwin: 不警告 sprintf() 之類的函數
#define _SCL_SECURE_NO_WARNINGS  //fonwin: 不警告 std::fill_n() 

/* fonwin: MSC 需要改一下 xtree:
bool operator==(const const_iterator& _Right) const {
// fonwin:有時會用(無效的iterator==有效的iterator)來判斷,所以不要產生exception!
// 把 #if _HAS_ITERATOR_DEBUGGING ... #endif 整個拿掉!
#if _HAS_ITERATOR_DEBUGGING
...
#endif
}
*/

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER				// Allow use of features specific to Windows XP or later.
#define WINVER 0x0501		// Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						

#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE			// Allow use of features specific to IE 6.0 or later.
#define _WIN32_IE 0x0600	// Change this to the appropriate value to target other versions of IE.
#endif

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

#endif
