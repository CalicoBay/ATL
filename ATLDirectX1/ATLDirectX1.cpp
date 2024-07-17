// ATLDirectX1.cpp : Implementation of WinMain


#include "pch.h"
#include "resource.h"
#include "MainWnd.h"

#pragma comment(linker, "/defaultlib:Comctl32.lib")
#pragma comment(lib, "runtimeobject.lib")


class CATLDirectX1Module : public ATL::CAtlExeModuleT<CATLDirectX1Module>
{
public:
	CDxMain* m_pCDxMain;

	HRESULT PreMessageLoop(int nCmdShow) throw()	
	{
		HRESULT hr = S_OK;
		//// call base first
		hr = __super::PreMessageLoop(nCmdShow);
		if(FAILED(hr))
		{
			return hr;
		}
		else
		{//RegisterClassObjects returns S_FALSE if there were no classes to register
			//but if PreMessageLoop doesn't return S_OK RunMessageLoop won't run
			hr = S_OK;
		}
		
		if(!::HeapSetInformation(__nullptr, HeapEnableTerminationOnCorruption, __nullptr, 0))
		{//If we can't set that somebody must have already screwed with this system
			return E_FAIL;
		}

		INITCOMMONCONTROLSEX icex;
		icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
		icex.dwICC = ICC_TAB_CLASSES;
		if(!InitCommonControlsEx(&icex))
		{
			return E_FAIL;
		}

		// Initialize the Windows Runtime.
		// This is needed for namespace DX in pch ReadDataAsync
		// ie:Microsoft::WRL::ComPtr<> doesn't need it.
		//Microsoft::WRL::Wrappers::RoInitializeWrapper initialize(RO_INIT_MULTITHREADED);
		//if (FAILED(initialize))
		//{
		//	return initialize;
		//}

		m_pCDxMain = new(std::nothrow) CDxMain;
		if(__nullptr == m_pCDxMain)
		{
			return E_OUTOFMEMORY;
		}
			
		//RECT rcPos = { CW_USEDEFAULT, 0, 0, 0 };
		hr = m_pCDxMain->Initialize();
		
		return hr;
	}

	void RunMessageLoop() throw()
	{
		MSG msg = {};
      while(GetMessage(&msg, 0, 0, 0) > 0)
      {
         ::TranslateMessage(&msg);
         ::DispatchMessage(&msg);
      }
		//while(WM_QUIT != msg.message)//(GetMessage(&msg, 0, 0, 0) > 0)
		//{
		//	if(::PeekMessage(&msg, __nullptr, 0, 0, PM_REMOVE))
		//	{
		//		//if(0 == ::TranslateAccelerator(msg.hwnd, m_hAccelTable, &msg))
		//		//{
		//			::TranslateMessage(&msg);
		//			::DispatchMessage(&msg);
		//		//}
		//	}
		//	else
		//	{
		//		m_pCDxMain->PaintCurrentTab();
		//	}
		//	//if(!::TranslateMDISysAccel(m_pFrame->m_hMDIClient, &msg))
		//	//{
		//	//	::TranslateMessage(&msg);
		//	//	::DispatchMessage(&msg);
		//	//}
		//}
	}

	HRESULT PostMessageLoop() throw()
	{
		HRESULT hr = __super::PostMessageLoop();
		if(__nullptr != m_pCDxMain)
		{
			delete m_pCDxMain;
			m_pCDxMain = __nullptr;
		}
		return hr;
	}

};

CATLDirectX1Module _AtlModule;

//extern "C" int WINAPI _tWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, LPTSTR /*lpCmdLine*/, int nShowCmd)
[Platform::MTAThreadAttribute]
int __stdcall _tWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, LPTSTR /*lpCmdLine*/, int nShowCmd)
{
	return _AtlModule.WinMain(nShowCmd);
}


