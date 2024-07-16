#pragma once
class CD2DTab : public CWindowImpl<CD2DTab, CWindow, CWinTraits<WS_CHILD|WS_VISIBLE>>
{
public:
	CD2DTab();
	~CD2DTab();
	DECLARE_WND_CLASS_EX(__nullptr, CS_DBLCLKS, -1);
	HRESULT Initialize(HWND hwndParent, LPCTSTR tszText = __nullptr);
protected:
	bool bAnimated;
	float m_dpiScaleX;
	float m_dpiScaleY;
	float m_Angle;
	float m_OriginX;
	float m_OriginY;
	CString m_sText0;
	//Direct2D
   Microsoft::WRL::ComPtr<ID2D1Factory> m_ComPtrD2DFactory;
   Microsoft::WRL::ComPtr<ID2D1HwndRenderTarget> m_ComPtrD2DRenderTarget;
   Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_ComPtrBlackBrush;
   Microsoft::WRL::ComPtr<ID2D1BitmapBrush> m_pGridPatternBrush;
	BEGIN_MSG_MAP(CD2DTab)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DISPLAYCHANGE, OnPaint)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
      MESSAGE_HANDLER(WM_TIMER, OnTimer)
	END_MSG_MAP()
	HRESULT CreateDeviceIndependentResources();
	HRESULT CreateDeviceResources();
	HRESULT CreateGridPatternBrush(ID2D1RenderTarget* pRenderTarget, ID2D1BitmapBrush** ppBitmapBrush);
	HRESULT Draw2D();
	VOID OnFinalMessage(HWND /*hwnd*/){delete this;}
	LRESULT OnCreate(UINT /*nMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnPaint(UINT /*nMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnSize(UINT /*nMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
   LRESULT OnTimer(UINT /*nMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/){ InvalidateRect(__nullptr, FALSE); return 0; }
};

