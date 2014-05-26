// PolyCtl.h : Declaration of the CPolyCtl
#pragma once
#include <math.h>
#include "resource.h"       // main symbols
#include <atlctl.h>
#include "Polygon_i.h"
#include "_IPolyCtlEvents_CP.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

using namespace ATL;



// CPolyCtl
class ATL_NO_VTABLE CPolyCtl :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CStockPropImpl<CPolyCtl, IPolyCtl>,
	public IOleControlImpl<CPolyCtl>,
	public IOleObjectImpl<CPolyCtl>,
	public IOleInPlaceActiveObjectImpl<CPolyCtl>,
	public IViewObjectExImpl<CPolyCtl>,
	public IOleInPlaceObjectWindowlessImpl<CPolyCtl>,
	public ISupportErrorInfo,
	public IConnectionPointContainerImpl<CPolyCtl>,
	public CProxy_IPolyCtlEvents<CPolyCtl>,
	public IQuickActivateImpl<CPolyCtl>,
#ifndef _WIN32_WCE
	public IDataObjectImpl<CPolyCtl>,
#endif
	public IProvideClassInfo2Impl<&CLSID_PolyCtl, &__uuidof(_IPolyCtlEvents), &LIBID_PolygonLib>,
	public IObjectSafetyImpl<CPolyCtl, INTERFACESAFE_FOR_UNTRUSTED_CALLER>,
	public CComCoClass<CPolyCtl, &CLSID_PolyCtl>,
	public CComControl<CPolyCtl>
{
public:


	CPolyCtl()
	{
		m_nSides = 3;
		m_clrFillColor = RGB(0, 255, 255);
	}

DECLARE_OLEMISC_STATUS(OLEMISC_RECOMPOSEONRESIZE |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_INSIDEOUT |
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST
)

DECLARE_REGISTRY_RESOURCEID(IDR_POLYCTL)


BEGIN_COM_MAP(CPolyCtl)
	COM_INTERFACE_ENTRY(IPolyCtl)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IViewObjectEx)
	COM_INTERFACE_ENTRY(IViewObject2)
	COM_INTERFACE_ENTRY(IViewObject)
	COM_INTERFACE_ENTRY(IOleInPlaceObjectWindowless)
	COM_INTERFACE_ENTRY(IOleInPlaceObject)
	COM_INTERFACE_ENTRY2(IOleWindow, IOleInPlaceObjectWindowless)
	COM_INTERFACE_ENTRY(IOleInPlaceActiveObject)
	COM_INTERFACE_ENTRY(IOleControl)
	COM_INTERFACE_ENTRY(IOleObject)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
	COM_INTERFACE_ENTRY(IConnectionPointContainer)
	COM_INTERFACE_ENTRY(IQuickActivate)
#ifndef _WIN32_WCE
	COM_INTERFACE_ENTRY(IDataObject)
#endif
	COM_INTERFACE_ENTRY(IProvideClassInfo)
	COM_INTERFACE_ENTRY(IProvideClassInfo2)
	COM_INTERFACE_ENTRY(IObjectSafety)
END_COM_MAP()

BEGIN_PROP_MAP(CPolyCtl)
	PROP_DATA_ENTRY("_cx", m_sizeExtent.cx, VT_UI4)
	PROP_DATA_ENTRY("_cy", m_sizeExtent.cy, VT_UI4)
#ifndef _WIN32_WCE
	PROP_ENTRY_TYPE("FillColor", DISPID_FILLCOLOR, CLSID_StockColorPage, VT_UI4)
#endif
	PROP_ENTRY_TYPE("Sides", 1, CLSID_PolyProp, VT_INT)
	// Example entries
	// PROP_ENTRY_TYPE("Property Name", dispid, clsid, vtType)
	// PROP_PAGE(CLSID_StockColorPage)
END_PROP_MAP()

BEGIN_CONNECTION_POINT_MAP(CPolyCtl)
	CONNECTION_POINT_ENTRY(__uuidof(_IPolyCtlEvents))
END_CONNECTION_POINT_MAP()

BEGIN_MSG_MAP(CPolyCtl)
	CHAIN_MSG_MAP(CComControl<CPolyCtl>)
	DEFAULT_REFLECTION_HANDLER()
	MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
END_MSG_MAP()
// Handler prototypes:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid)
	{
		static const IID* const arr[] =
		{
			&IID_IPolyCtl,
		};

		for (int i=0; i<sizeof(arr)/sizeof(arr[0]); i++)
		{
			if (InlineIsEqualGUID(*arr[i], riid))
				return S_OK;
		}
		return S_FALSE;
	}

// IViewObjectEx
	DECLARE_VIEW_STATUS(VIEWSTATUS_SOLIDBKGND | VIEWSTATUS_OPAQUE)

// IPolyCtl
public:
	HRESULT OnDraw(ATL_DRAWINFO& di)
	{
//		//Wizard code
//		RECT& rc = *(RECT*)di.prcBounds;
//		// Set Clip region to the rectangle specified by di.prcBounds
//		HRGN hRgnOld = NULL;
//		if (GetClipRgn(di.hdcDraw, hRgnOld) != 1)
//			hRgnOld = NULL;
//		bool bSelectOldRgn = false;
//
//		HRGN hRgnNew = CreateRectRgn(rc.left, rc.top, rc.right, rc.bottom);
//
//		if (hRgnNew != NULL)
//		{
//			bSelectOldRgn = (SelectClipRgn(di.hdcDraw, hRgnNew) != ERROR);
//		}
//
//		Rectangle(di.hdcDraw, rc.left, rc.top, rc.right, rc.bottom);
//		SetTextAlign(di.hdcDraw, TA_CENTER|TA_BASELINE);
//		LPCTSTR pszText = _T("PolyCtl");
//#ifndef _WIN32_WCE
//		TextOut(di.hdcDraw,
//			(rc.left + rc.right) / 2,
//			(rc.top + rc.bottom) / 2,
//			pszText,
//			lstrlen(pszText));
//#else
//		ExtTextOut(di.hdcDraw,
//			(rc.left + rc.right) / 2,
//			(rc.top + rc.bottom) / 2,
//			ETO_OPAQUE,
//			NULL,
//			pszText,
//			ATL::lstrlen(pszText),
//			NULL);
//#endif
//
//		if (bSelectOldRgn)
//			SelectClipRgn(di.hdcDraw, hRgnOld);
//
//		DeleteObject(hRgnNew);
//
//		return S_OK;
		RECT& rc = *(RECT*)di.prcBounds;
		HDC hdc = di.hdcDraw;

		COLORREF    colFore;
		HBRUSH      hOldBrush, hBrush;
		HPEN        hOldPen, hPen;

		// Translate m_colFore into a COLORREF type
		OleTranslateColor(m_clrFillColor, NULL, &colFore);

		// Create and select the colors to draw the circle
		hPen = (HPEN)GetStockObject(BLACK_PEN);
		hOldPen = (HPEN)SelectObject(hdc, hPen);
		hBrush = (HBRUSH)GetStockObject(WHITE_BRUSH);
		hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);

		Ellipse(hdc, rc.left, rc.top, rc.right, rc.bottom);

		// Create and select the brush that will be used to fill the polygon
		hBrush = CreateSolidBrush(colFore);
		SelectObject(hdc, hBrush);

		CalcPoints(rc);
		Polygon(hdc, &m_arrPoint[0], m_nSides);

		// Select back the old pen and brush and delete the brush we created
		SelectObject(hdc, hOldPen);
		SelectObject(hdc, hOldBrush);
		DeleteObject(hBrush);

		return S_OK;
	}

	OLE_COLOR m_clrFillColor;
	short m_nSides;
	POINT m_arrPoint[100];
	void OnFillColorChanged()
	{
		ATLTRACE(_T("OnFillColorChanged\n"));
	}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}
	STDMETHOD(get_Sides)(SHORT* pVal);
	STDMETHOD(put_Sides)(SHORT newVal);
	void CalcPoints(const RECT& rc);
	LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/);
};

OBJECT_ENTRY_AUTO(__uuidof(PolyCtl), CPolyCtl)
