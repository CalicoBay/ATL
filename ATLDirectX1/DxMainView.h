#pragma once
//CFrameWinTraits with Scrolling added
class CDxMainView : public CWindowImpl<CDxMainView, CWindow, CWinTraits<WS_OVERLAPPEDWINDOW|WS_VSCROLL|WS_HSCROLL|WS_CLIPCHILDREN|WS_CLIPSIBLINGS,WS_EX_APPWINDOW|WS_EX_WINDOWEDGE>>
//class CDxMainView : public CWindowImpl<CDxMainView, CWindow, CFrameWinTraits>
{
public:
	CDxMainView();
	~CDxMainView();
	void  GetInverseViewMatrix(OUT DWRITE_MATRIX* matrix) const;
	void PostRedraw() { InvalidateRect(NULL, FALSE); }
protected:
	float m_dpiScaleX;
	float m_dpiScaleY;
	float m_Angle;
	float m_OriginX;
	float m_OriginY;
	CString m_sText0;
	//Direct2D
	ID2D1Factory* m_pD2DFactory;
	ID2D1HwndRenderTarget* m_pD2DRenderTarget;
	ID2D1SolidColorBrush* m_pBlackBrush;
	ID2D1BitmapBrush* m_pGridPatternBrush;
	//DirectWrite
	IDWriteFactory* m_pDWriteFactory;
	IDWriteTextFormat* m_pDWriteTextFormat0;
	IDWriteTextLayout* m_pDWriteTextLayout0;
	BEGIN_MSG_MAP(CDxMain)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DISPLAYCHANGE, OnDisplayChange)
		COMMAND_ID_HANDLER(IDM_ABOUT, OnAboutBox)
		COMMAND_ID_HANDLER(IDM_EXIT, OnExit)
		MESSAGE_HANDLER(WM_HSCROLL, OnScroll)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_VSCROLL, OnScroll)
	END_MSG_MAP()
	VOID ConstrainViewOrigin();
	HRESULT CreateDeviceIndependentResources();
	HRESULT CreateDeviceResources();
	HRESULT CreateGridPatternBrush(ID2D1RenderTarget* pRenderTarget, ID2D1BitmapBrush** ppBitmapBrush);
	HRESULT Draw2D();
	VOID DrawText0();
	VOID DiscardDeviceResources();
	VOID DiscardDeviceIndependentResources();
	D2D1_POINT_2F GetPageSize(IDWriteTextLayout* pTextLayout);
	LRESULT OnCreate(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnDisplayChange(UINT /*nMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnScroll(UINT /*nMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnPaint(UINT /*nMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnSize(UINT /*nMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnAboutBox(WORD , WORD , HWND , BOOL& )
	{
		CString sTitle;
		sTitle.LoadStringW(IDS_PROJNAME);
		::ShellAbout(m_hWnd, sTitle, NULL, NULL);
		return 0;
	}
	LRESULT OnExit(WORD , WORD , HWND , BOOL&)
	{
		DestroyWindow();
		return 0;
	}
	void OnFinalMessage(HWND /*hWnd*/)
	{
		::PostQuitMessage(0);
	}
};


