#pragma once
interface IDrawingEffect;
interface IRenderTarget;
interface IInlineImage;
class CDWriteTab : public CWindowImpl<CDWriteTab, CWindow, CWinTraits<WS_CHILD|WS_VISIBLE|WS_HSCROLL|WS_VSCROLL>>
{
public:
	CDWriteTab();
   ~CDWriteTab(){};
	DECLARE_WND_CLASS_EX(__nullptr, CS_DBLCLKS | CS_VREDRAW | CS_HREDRAW, -1);
	HRESULT Initialize(HWND hwndParent, LPCTSTR tszText = __nullptr);
	void  GetInverseViewMatrix(OUT DWRITE_MATRIX* matrix) const;
   void  GetViewMatrix(OUT DWRITE_MATRIX* matrix) const;
	void PostRedraw() { InvalidateRect(__nullptr, FALSE); }
   enum SetSelectionMode
   {
      SetSelectionModeLeft,               // cluster left
      SetSelectionModeRight,              // cluster right
      SetSelectionModeUp,                 // line up
      SetSelectionModeDown,               // line down
      SetSelectionModeLeftChar,           // single character left (backspace uses it)
      SetSelectionModeRightChar,          // single character right
      SetSelectionModeLeftWord,           // single word left
      SetSelectionModeRightWord,          // single word right
      SetSelectionModeHome,               // front of line
      SetSelectionModeEnd,                // back of line
      SetSelectionModeFirst,              // very first position
      SetSelectionModeLast,               // very last position
      SetSelectionModeAbsoluteLeading,    // explicit position (for mouse click)
      SetSelectionModeAbsoluteTrailing,   // explicit position, trailing edge
      SetSelectionModeAll                 // select all text
   };
protected:
	bool m_bAnimated;
   bool m_currentlySelecting : 1;
   bool m_currentlyPanning : 1;
   float m_previousMouseX;
   float m_previousMouseY;
   float m_dpiScaleX;
	float m_dpiScaleY;
   float m_viewScaleX;
   float m_viewScaleY;
	float m_Angle;
	float m_OriginX;
	float m_OriginY;
   D2D1_RECT_F m_rcClientF;
   EditableLayout m_layoutEditor;
   HWND m_hWndParent;
   ////////////////////
   // Selection/Caret navigation
   ///
   // caretAnchor equals caretPosition when there is no selection.
   // Otherwise, the anchor holds the point where shift was held
   // or left drag started.
   //
   // The offset is used as a sort of trailing edge offset from
   // the caret position. For example, placing the caret on the
   // trailing side of a surrogate pair at the beginning of the
   // text would place the position at zero and offset of two.
   // So to get the absolute leading position, sum the two.
   UINT32 m_caretAnchor;
   UINT32 m_caretPosition;
   UINT32 m_caretPositionOffset;    // > 0 used for trailing edge
   // Current attributes of the caret, which can be independent
   // of the text.
   EditableLayout::CaretFormat m_caretFormat;
   int m_iUTF;
   enum { MouseScrollFactor = 10 };
	CStringW m_sCreateText;
	CStringW m_sInitText;
	CStringA m_sAnsi;
	CStringW m_sUnicode;
   //IWIC
   CComPtr<IWICImagingFactory> m_CComPtrIWICImagingFactory;
   //Direct2D
   CComPtr<ID2D1Factory1> m_CComPtrD2DFactory1;
   //DirectWrite
   CComPtr<IDWriteFactory7> m_CComPtrDWriteFactory7;
   CComPtr<IDWriteTextFormat> m_CComPtrDWriteTextFormat0;
   CComPtr<IDWriteTextLayout> m_CComPtrDWriteTextLayout0;
   //CalicoBay
   CComPtr<IDrawingEffect> m_CComPtrPageBackgroundEffect;
   CComPtr<IDrawingEffect> m_CComPtrTextSelectionEffect;
   CComPtr<IDrawingEffect> m_CComPtrImageSelectionEffect;
   CComPtr<IDrawingEffect> m_CComPtrCaretBackgroundEffect;
   CComPtr<IDrawingEffect> m_CComPtrReuseableEffect;
   CComPtr<IRenderTarget> m_CComPtrRenderTarget;
   BEGIN_MSG_MAP(CDWriteTab)
      MESSAGE_HANDLER(WM_CAPTURECHANGED, OnMouseExit)
      MESSAGE_HANDLER(WM_CHAR, OnKeyCharacter)
      MESSAGE_HANDLER(WM_CONTEXTMENU, OnContextMenu)
      MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DISPLAYCHANGE, OnDisplayChange)
      MESSAGE_HANDLER(WM_KEYDOWN, OnKeyPress)
      MESSAGE_HANDLER(WM_KILLFOCUS, OnKillFocus)
      MESSAGE_HANDLER(WM_LBUTTONDOWN, OnMousePress)
      MESSAGE_HANDLER(WM_LBUTTONDBLCLK, OnMousePress)
      MESSAGE_HANDLER(WM_LBUTTONUP, OnMouseRelease)
      MESSAGE_HANDLER(WM_MBUTTONDBLCLK, OnMousePress)
      MESSAGE_HANDLER(WM_MBUTTONDOWN, OnMousePress)
      MESSAGE_HANDLER(WM_MBUTTONUP, OnMouseRelease)
      MESSAGE_HANDLER(WM_MOUSEHWHEEL, OnMouseScroll)
      MESSAGE_HANDLER(WM_MOUSELEAVE, OnMouseExit)
      MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
      MESSAGE_HANDLER(WM_MOUSEWHEEL, OnMouseScroll)
      MESSAGE_HANDLER(WM_PAINT, OnPaint)
      MESSAGE_HANDLER(WM_RBUTTONDBLCLK, OnMousePress)
      MESSAGE_HANDLER(WM_RBUTTONDOWN, OnMousePress)
      MESSAGE_HANDLER(WM_RBUTTONUP, OnMouseRelease)
      MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)
      MESSAGE_HANDLER(WM_SIZE, OnSize)
      MESSAGE_HANDLER(WM_TIMER, OnTimer)
      MESSAGE_HANDLER(WM_VSCROLL, OnScroll)
      MESSAGE_HANDLER(WM_HSCROLL, OnScroll)
	END_MSG_MAP()
   VOID AlignCaretToNearestCluster(bool isTrailingHit = false, bool skipZeroWidth = false);
	VOID ConstrainViewOrigin();
   VOID CopyToClipboard();
	HRESULT CreateDeviceIndependentResources();
	HRESULT CreateDeviceResources();
   VOID DeleteSelection();
   HRESULT DrawPage(IRenderTarget* pIRenderTarget);
   UINT32 GetCaretPosition(){ return m_caretPosition + m_caretPositionOffset; }
   VOID GetCaretRect(D2D1_RECT_F& caretRect);
   void GetLineFromPosition(const DWRITE_LINE_METRICS* lineMetrics, UINT32 lineCount, UINT32 textPosition, OUT UINT32* lineOut, OUT UINT32* linePositionOut);
   VOID GetLineMetrics(OUT std::vector<DWRITE_LINE_METRICS>& lineMetrics);
   D2D1_POINT_2F GetPageSize(IDWriteTextLayout* pTextLayout);
   VOID GetScale(float* pScaleX, float* pScaleY){ *pScaleX = m_viewScaleX; *pScaleY = m_viewScaleY; }
   DWRITE_TEXT_RANGE GetSelectionRange();
   HRESULT InsertText(LPCWSTR wszText, UINT32 cch);
   int IsUnicode(Platform::Array<byte>^ fileData);
   VOID MirrorXCoordinate(IN OUT float& x);
   HRESULT OpenImageFile(LPCWSTR wszPath);
   VOID PasteFromClipboard();
   VOID RefreshView();
   VOID SetScale(float scaleX, float scaleY, bool bRelative);
   bool SetSelection(SetSelectionMode moveMode, UINT32 advance, bool extendSelection, bool updateCaretFormat = true);
   bool SetSelectionFromPoint(float x, float y, bool extendSelection);
   VOID UpdateCaretFormatting();
   VOID UpdateScrollInfo();
   VOID UpdateSystemCaret(const D2D1_RECT_F& rect);
   LRESULT OnContextMenu(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
   LRESULT OnCreate(UINT, WPARAM, LPARAM, BOOL&);
   LRESULT OnDisplayChange(UINT /*nMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
   LRESULT OnKeyCharacter(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
   LRESULT OnKeyPress(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
   LRESULT OnKillFocus(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
   LRESULT OnMouseExit(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
   LRESULT OnMouseMove(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
   LRESULT OnMousePress(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
   LRESULT OnMouseRelease(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
   LRESULT OnMouseScroll(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
   LRESULT OnPaint(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
   LRESULT OnScroll(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
   LRESULT OnSetFocus(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
   LRESULT OnSetInlineImage(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
   LRESULT OnSize(UINT /*nMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
   LRESULT OnTimer(UINT /*nMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/){ InvalidateRect(__nullptr, FALSE); return 0; }
   VOID OnFinalMessage(HWND /*hwnd*/){ delete this; }
public:
   // Resets the default view
   void ResetView();
};


