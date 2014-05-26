// ExePolygon.cpp : Implementation of WinMain


#include "stdafx.h"
#include "resource.h"
#include "ExePolygon_i.h"
#include "xdlldata.h"
#include "MainWnd.h"


using namespace ATL;


class CExePolygonModule : public ATL::CAtlExeModuleT< CExePolygonModule >
{
public :
	DECLARE_LIBID(LIBID_ExePolygonLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_EXEPOLYGON, "{5E3CA08A-93D8-4D78-8886-7001A8578E88}")
	HRESULT PreMessageLoop(int nCmdShow) throw();
	VOID RunMessageLoop() throw();
	HRESULT PostMessageLoop() throw(){ return __super::PostMessageLoop(); }
};

HRESULT CExePolygonModule::PreMessageLoop(int nCmdShow) throw()
{
	HRESULT hr = __super::PreMessageLoop(nCmdShow);
	if (FAILED(hr))
	{
		return hr;
	}
	CComPtr<IMainWnd> cpMainWnd;
	hr = cpMainWnd.CoCreateInstance(CLSID_MainWnd);
	if (SUCCEEDED(hr))
	{
		IMainWnd* pIMainWnd = __nullptr;
		hr = cpMainWnd->QueryInterface(&pIMainWnd);
		if (SUCCEEDED(hr))
		{
			RECT rcPos = { CW_USEDEFAULT, 0, 0, 0 };
			hr = pIMainWnd->CreateMain(__nullptr, rcPos, _T("The Main"), 0, 0, __nullptr, (LPARAM)__nullptr);
			if (SUCCEEDED(hr))
			{
				//TODO: PostQuitMessage() and more.
				::MessageBox(__nullptr, _T("Success obtaining IMainWnd!"), _T(""), MB_OK);
			}
		}
	}
	return hr;
}

VOID CExePolygonModule::RunMessageLoop() throw()
{
	MSG msg = {};
	while((::GetMessage(&msg, 0, 0, 0) > 0))
	{
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}
}

CExePolygonModule _AtlModule;



//
extern "C" int WINAPI _tWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, 
								LPTSTR /*lpCmdLine*/, int nShowCmd)
{
	return _AtlModule.WinMain(nShowCmd);
}

