// MainWnd.h : Declaration of the CMainWnd

#pragma once
#include "resource.h"       // main symbols



#include "ExePolygon_i.h"
#include "_IMainWndEvents_CP.h"



#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

using namespace ATL;

class CAtlEdit : public CWindowImpl < CAtlEdit, CWindow, CWinTraits<WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL> >
{
public:
	DECLARE_WND_SUPERCLASS(_T("CAtlEdit"), _T("EDIT"))
	BEGIN_MSG_MAP(CAtlEdit)
	END_MSG_MAP()
};

// CMainWnd
class ATL_NO_VTABLE CMainWnd :
	public CWindowImpl<CMainWnd, CWindow, CFrameWinTraits>,
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CMainWnd, &CLSID_MainWnd>,
	public IConnectionPointContainerImpl<CMainWnd>,
	public CProxy_IMainWndEvents<CMainWnd>,
	public IMainWnd
{
public:
	CMainWnd() : m_hwndEdit(__nullptr)
	{
	}
	DECLARE_WND_CLASS(_T("CMAINWND"))
	BEGIN_MSG_MAP(CMainWnd)
      MESSAGE_HANDLER(WM_CREATE, OnCreate)
      MESSAGE_HANDLER(WM_SIZE, OnSize)
   END_MSG_MAP()
   LRESULT OnCreate(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
   LRESULT OnSize(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
   void OnFinalMessage(HWND hWnd)
   {
      ::PostQuitMessage(0);
   }

DECLARE_REGISTRY_RESOURCEID(IDR_MAINWND)

DECLARE_NOT_AGGREGATABLE(CMainWnd)

BEGIN_COM_MAP(CMainWnd)
	COM_INTERFACE_ENTRY(IMainWnd)
	COM_INTERFACE_ENTRY(IConnectionPointContainer)
END_COM_MAP()

BEGIN_CONNECTION_POINT_MAP(CMainWnd)
	CONNECTION_POINT_ENTRY(__uuidof(_IMainWndEvents))
END_CONNECTION_POINT_MAP()


	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}
	STDMETHOD(CreateMain)(HWND hWndParent, RECT rect, LPCTSTR szWindowName, DWORD dwStyle, DWORD dwExStyle, HMENU MenuOrID, LPARAM lpCreateParam);
public:
	HWND m_hwndEdit;


};

OBJECT_ENTRY_AUTO(__uuidof(MainWnd), CMainWnd)
