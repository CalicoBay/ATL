// pch.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#ifndef STRICT
#define STRICT
#endif

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <shellapi.h>
#include <ShObjIdl.h> // for IFileDialogEvents
#include <commdlg.h>
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit


#define ATL_NO_ASSERT_ON_DESTROY_NONEXISTENT_WINDOW

#include "resource.h"
#include <atlbase.h>
#include <atlfile.h>
#include <atltypes.h>
#include <atlwin.h>
#include <atlstr.h>
#include <vector>
#include <algorithm>
#include <functional>
#include <strsafe.h>
// Com and WinRT headers
#include <Unknwn.h>
#include <winrt/base.h>
#include "AtlControls.h"

#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "propsys.lib")
// set up common controls v6 the easy way
#pragma comment(linker, "\"/manifestdependency:type='Win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

// retrieve the HINSTANCE for the current DLL or EXE using this symbol that
// the linker provides for every module, avoids the need for a global HINSTANCE variable
// and provides access to this value for static libraries
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
__inline HINSTANCE GetModuleHINSTANCE() { return (HINSTANCE)&__ImageBase; }
