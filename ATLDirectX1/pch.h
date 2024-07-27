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
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit
#define _ATL_NO_HOSTING


#include "resource.h"
#include <atlbase.h>
#include <atlcom.h>
#include <atlcomcli.h>
//#include <atlhost.h>
#include <atlctl.h>
#include <afxres.h>
#include <atltypes.h>
#include <atlwin.h>
#include <atlstr.h>
#include <d2d1_1.h>
#include <d2d1_1helper.h>
#include <d3d11_1.h>
#include <dwrite_3.h>
#include <dxgi1_2.h>
#include <DirectXMath.h>
#include <wincodec.h>
#include <vector>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <ShObjIdl.h>
//#include <Windows.Foundation.h>
//#include <Windows.System.Threading.h>
#include <wrl/wrappers/corewrappers.h>
#include <wrl/client.h>
#include <ppl.h>
#include <ppltasks.h>
//////////////////////////////////
// Testing Cpp/WinRT
//#include <winrt/base.h>
//#include <winrt/Windows.Foundation.h>
//#include <winrt/Windows.Foundation.Collections.h>
//#include <winrt/Windows.UI.Composition.h>
//#include <winrt/Windows.Web.h>
//#include <winrt/Windows.Web.Syndication.h>
///////////////////////////////////
// retrieve the HINSTANCE for the current DLL or EXE using this symbol that
// the linker provides for every module, avoids the need for a global HINSTANCE variable
// and provides access to this value for static libraries
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
__inline HINSTANCE GetModuleHINSTANCE() { return (HINSTANCE)&__ImageBase; }

template <class T> inline T* SafeAcquire(T* pEffect)
{
   if(__nullptr != pEffect)
   {
      pEffect->AddRef();
   }
   return pEffect;
}


template <class T> inline void SafeRelease(T **ppT)
{
	if(NULL != *ppT)
	{
		(*ppT)->Release();
		*ppT = NULL;
	}
}

namespace DX
{
	void ThrowIfFailed(HRESULT hr);
	void ThrowIfFALSE(BOOL bResult);
	// Function that reads from a binary file asynchronously.
	Concurrency::task<Platform::Array<byte>^> CreateReadDataAsync(Platform::String^ filename);
	Platform::Array<byte>^ ReadDataAsync(Platform::String^ filename);
	Windows::Storage::StorageFolder^ ReturnCurrentFolderAsync();
   void ComputeInverseMatrix(DWRITE_MATRIX const& matrix, OUT DWRITE_MATRIX& result);
   D2D1_POINT_2F GetPageSize(IDWriteTextLayout* textLayout);
   bool IsLandscapeAngle(float angle);
   inline D2D1::Matrix3x2F& Cast(DWRITE_MATRIX& matrix)
   {
      // DWrite's matrix, D2D's matrix, and GDI's XFORM
      // are all compatible.
      return *reinterpret_cast<D2D1::Matrix3x2F*>(&matrix);
   }

   inline DWRITE_MATRIX& Cast(D2D1::Matrix3x2F& matrix)
   {
      return *reinterpret_cast<DWRITE_MATRIX*>(&matrix);
   }

   inline int RoundToInt(float x)
   {
      return static_cast<int>(floor(x + .5));
   }

   inline double DegreesToRadians(float degrees)
   {
      return degrees * DirectX::XM_PI * 2.0f / 360.0f;
   }

   inline float GetDeterminant(DWRITE_MATRIX const& matrix)
   {
      return matrix.m11 * matrix.m22 - matrix.m12 * matrix.m21;
   }

   // Needed text editor backspace deletion.
   inline bool IsSurrogate(UINT32 ch) throw()
   {
      // 0xD800 <= ch <= 0xDFFF
      return (ch & 0xF800) == 0xD800;
   }


   inline bool IsHighSurrogate(UINT32 ch) throw()
   {
      // 0xD800 <= ch <= 0xDBFF
      return (ch & 0xFC00) == 0xD800;
   }

   inline bool IsLowSurrogate(UINT32 ch) throw()
   {
      // 0xDC00 <= ch <= 0xDFFF
      return (ch & 0xFC00) == 0xDC00;
   }

   HRESULT STDMETHODCALLTYPE LoadImageFromResource(LPCWSTR wszResourceName, LPCWSTR wszResourceType, IWICImagingFactory* pIWICImagingFactory, IWICBitmapSource** ppIWICBitmapSource);
   HRESULT STDMETHODCALLTYPE LoadImageFromFile(LPCWSTR wszFileName, IWICImagingFactory* pIWICImagingFactory, IWICBitmapSource** ppIWICBitmapSource);
   HRESULT STDMETHODCALLTYPE LoadShaderFromResource(LPCWSTR wszResourceName, PUINT8& pBuffer, size_t& uiCount);
}

