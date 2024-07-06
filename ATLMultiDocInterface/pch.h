// stdafx.h : include file for standard system include files,
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
#define _ATL_NO_COM_SUPPORT
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit
#define ATL_NO_ASSERT_ON_DESTROY_NONEXISTENT_WINDOW

#include "resource.h"
#include <atlbase.h>
#include <atltypes.h>
#include <atlwin.h>
#include <atlstr.h>
#include <vector>
#include <algorithm>
#include <functional>

// retrieve the HINSTANCE for the current DLL or EXE using this symbol that
// the linker provides for every module, avoids the need for a global HINSTANCE variable
// and provides access to this value for static libraries
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
__inline HINSTANCE GetModuleHINSTANCE() { return (HINSTANCE)&__ImageBase; }
// This tells App.xaml.g.hpp the app will define its own WinMain
#define DISABLE_XAML_GENERATED_MAIN

// There's an API named GetCurrentTime in the Storyboard type.
#undef GetCurrentTime

// Com and WinRT headers
#include <Unknwn.h>

// Some generated files, like MainPage.xaml.g.h need files such as Microsoft.UI.Xaml.Markup.h
// to already be included.  Let's just include a bunch of stuff we know we'll need here in the PCH.
#include <winrt/Microsoft.UI.Content.h>
#include <winrt/Microsoft.UI.Dispatching.h>
#include <winrt/Microsoft.UI.Interop.h>
#include <winrt/Microsoft.UI.Xaml.Controls.Primitives.h>
#include <winrt/Microsoft.UI.Xaml.h>
#include <winrt/Microsoft.UI.Xaml.Hosting.h>
#include <winrt/Microsoft.UI.Xaml.Markup.h>
#include <winrt/Windows.Foundation.h>
