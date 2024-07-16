#include "stdafx.h"
#include "DxMainView.h"

#pragma comment(linker, "/defaultlib:d3d11.lib")
#pragma comment(linker, "/defaultlib:d2d1.lib")
#pragma comment(linker, "/defaultlib:dwrite.lib")

CDxMainView::CDxMainView() :
	m_dpiScaleX(1.0),
	m_dpiScaleY(1.0),
	m_Angle(0),
	m_OriginX(0),
	m_OriginY(0),
	m_pBlackBrush(NULL),
	m_pGridPatternBrush(NULL),
	m_pD2DFactory(NULL),
	m_pD2DRenderTarget(NULL),
	m_pDWriteFactory(NULL),
	m_pDWriteTextFormat0(NULL),
	m_pDWriteTextLayout0(NULL)
{
}

CDxMainView::~CDxMainView()
{
	DiscardDeviceResources();
	DiscardDeviceIndependentResources();
}

VOID CDxMainView::DiscardDeviceResources()
{
//Direct2D
	::SafeRelease(&m_pD2DRenderTarget);
	::SafeRelease(&m_pBlackBrush);
	::SafeRelease(&m_pGridPatternBrush);
}

VOID CDxMainView::DiscardDeviceIndependentResources()
{
//Direct2D
	::SafeRelease(&m_pD2DFactory);
	//DirectWrite
	::SafeRelease(&m_pDWriteFactory);
	::SafeRelease(&m_pDWriteTextFormat0);
	::SafeRelease(&m_pDWriteTextLayout0);
}

LRESULT CDxMainView::OnCreate(UINT /*nMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
    LRESULT lResult;
	CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
	if(NULL != pCreate)
	{
		LPCTSTR tszText0 = (LPCTSTR)pCreate->lpCreateParams;
		m_sText0.Format(_T("%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s"), tszText0, tszText0, tszText0, tszText0, tszText0, tszText0);
		lResult = 0;
	}
	else
	{
		lResult = -1;
	}

	return lResult;
}

LRESULT CDxMainView::OnDisplayChange(UINT /*nMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	InvalidateRect(NULL, FALSE);

	return 0;
}

LRESULT CDxMainView::OnPaint(UINT /*nMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	//PAINTSTRUCT ps;
	//RECT rcClient;

	//BeginPaint(&ps);
		//::TextOut(GetDC(), 0, 0, _T("TODO: Something with this space!"), 32);
		Draw2D();
		ValidateRect(NULL);
	//EndPaint(&ps);

	return 0;
}

LRESULT CDxMainView::OnSize(UINT /*nMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	//POINT caretPos = {0};
	D2D1_POINT_2F pageSize = {0};
	D2D1_SIZE_U uSize;
	D2D1_SIZE_F flSize;
	SCROLLINFO scrollInfo = {sizeof(scrollInfo)};

	//GetCaretPos(&caretPos);

	uSize.width = LOWORD(lParam);
	uSize.height = HIWORD(lParam);
	flSize.height = uSize.height / m_dpiScaleY;
	flSize.width = uSize.width / m_dpiScaleX;

	if(NULL != m_pD2DRenderTarget)
	{

		m_pD2DRenderTarget->Resize(uSize);
		if(NULL != m_pDWriteTextLayout0)
		{
			pageSize = GetPageSize(m_pDWriteTextLayout0);
			m_pDWriteTextLayout0->SetMaxWidth(flSize.width);
			m_pDWriteTextLayout0->SetMaxHeight(flSize.height);
		}


		InvalidateRect(NULL, FALSE);
	}

	scrollInfo.fMask    = SIF_PAGE|SIF_POS|SIF_RANGE;
	//Set vertical bar
	scrollInfo.nPage = pageSize.y > 0 ? pageSize.y : flSize.height;//int(::abs(flSize.height));
	scrollInfo.nPos = pageSize.y > uSize.height ? pageSize.y - uSize.height : uSize.height;//int(flSize.height >= 0 ? caretPos.y : uSize.height - caretPos.y);
	scrollInfo.nMin = 0;
	scrollInfo.nMax = int(uSize.height) + scrollInfo.nPage;
	SetScrollInfo(SB_VERT, &scrollInfo, TRUE);
	scrollInfo.nPos = 0;
	scrollInfo.nMax = 0;
	GetScrollInfo(SB_VERT,&scrollInfo);
	//Set horizontal bar
	scrollInfo.nPage = pageSize.x > 0 ? pageSize.x : flSize.width;
	scrollInfo.nPos = pageSize.x > uSize.width ? pageSize.x - uSize.width : uSize.width;
	scrollInfo.nMin = 0;
	scrollInfo.nMax = int(uSize.width) + scrollInfo.nPage;
	SetScrollInfo(SB_HORZ, &scrollInfo, TRUE);

	return 0;
}

LRESULT CDxMainView::OnScroll(UINT nMsg, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    // Reacts to scroll bar changes.

    UINT uiRequest = LOWORD(wParam);
	SCROLLINFO scrollInfo = {sizeof(scrollInfo)};
    scrollInfo.fMask = SIF_ALL;
 
    int barOrientation = (nMsg == WM_VSCROLL) ? SB_VERT : SB_HORZ;

    if(!GetScrollInfo(barOrientation, &scrollInfo))
	{
		return 0;
	}
    // Save the position for comparison later on
    int oldPosition = scrollInfo.nPos;

    switch (uiRequest)
    {
    case SB_TOP:        scrollInfo.nPos  = scrollInfo.nMin;      break;
    case SB_BOTTOM:     scrollInfo.nPos  = scrollInfo.nMax;      break;
    case SB_LINEUP:     scrollInfo.nPos -= 10;                   break;
    case SB_LINEDOWN:   scrollInfo.nPos += 10;                   break;
    case SB_PAGEUP:     scrollInfo.nPos -= scrollInfo.nPage;     break;
    case SB_PAGEDOWN:   scrollInfo.nPos += scrollInfo.nPage;     break;
    case SB_THUMBTRACK: scrollInfo.nPos  = scrollInfo.nTrackPos; break;
    default:
         break;
    }

    if(scrollInfo.nPos < 0)
	{
        scrollInfo.nPos = 0;
	}
    if(scrollInfo.nPos > scrollInfo.nMax - signed(scrollInfo.nPage))
	{
        scrollInfo.nPos = scrollInfo.nMax - scrollInfo.nPage;
	}

    scrollInfo.fMask = SIF_POS;
    SetScrollInfo(barOrientation, &scrollInfo, TRUE);

    // If the position has changed, scroll the window 
    if (scrollInfo.nPos != oldPosition)
    {
        // Need the view matrix in case the editor is flipped/mirrored/rotated.
        //D2D1::Matrix3x2F pageTransform;
		//GetInverseViewMatrix(&Cast(pageTransform));
		PostRedraw();
    }

	return 0;
}

VOID CDxMainView::ConstrainViewOrigin()
{
}

HRESULT CDxMainView::CreateDeviceIndependentResources()
{
	HRESULT hResult = ::D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pD2DFactory);
	if(SUCCEEDED(hResult))
	{
		hResult = ::DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
												reinterpret_cast<IUnknown**>(&m_pDWriteFactory));

	}
	if(SUCCEEDED(hResult))
	{
		hResult = m_pDWriteFactory->CreateTextFormat(L"Gabriola", NULL, DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 72.0f, L"en-US", &m_pDWriteTextFormat0);
	}
	if(SUCCEEDED(hResult))
	{
		hResult = m_pDWriteTextFormat0->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	}
	if(SUCCEEDED(hResult))
	{
		hResult = m_pDWriteTextFormat0->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	}

	m_dpiScaleX = ::GetDeviceCaps(GetDC(), LOGPIXELSX) / 96.0f;
	m_dpiScaleY = ::GetDeviceCaps(GetDC(), LOGPIXELSY) / 96.0f;

	if(SUCCEEDED(hResult))
	{
		RECT clientRect;
		GetClientRect(&clientRect);
		float width = clientRect.right / m_dpiScaleX;//816.0f
		float height = clientRect.bottom / m_dpiScaleY;//1056.0f

		hResult = m_pDWriteFactory->CreateTextLayout(m_sText0, m_sText0.GetLength(), m_pDWriteTextFormat0, width, height, &m_pDWriteTextLayout0); 
	}

	return hResult;
}

HRESULT CDxMainView::CreateDeviceResources()
{
	HRESULT hResult = S_OK;
	RECT clientRect;
	GetClientRect(&clientRect);

	D2D1_SIZE_U size = D2D1::SizeU(clientRect.right - clientRect.left, clientRect.bottom - clientRect.top);
	//D2D1_SIZE_U size = D2D1::SizeU(816, 1056);
	//Create a Direct2D render target
	if(NULL == m_pD2DRenderTarget)
	{
		hResult = m_pD2DFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(),
														D2D1::HwndRenderTargetProperties(m_hWnd, size),
														&m_pD2DRenderTarget);
		//Create a Black Brush
		if(SUCCEEDED(hResult))
		{
			hResult = m_pD2DRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &m_pBlackBrush);
		}
		//Create the Grid Brush
		if(SUCCEEDED(hResult))
		{
			hResult = CreateGridPatternBrush(m_pD2DRenderTarget, &m_pGridPatternBrush);
		}
	}

	return hResult;
}

HRESULT CDxMainView::CreateGridPatternBrush(ID2D1RenderTarget* pRenderTarget, ID2D1BitmapBrush** ppBitmapBrush)
{
	HRESULT hResult = S_OK;

    // Create a compatible render target.
    ID2D1BitmapRenderTarget *pCompatibleRenderTarget = NULL;
    hResult = pRenderTarget->CreateCompatibleRenderTarget(
        D2D1::SizeF(10, 10),
        &pCompatibleRenderTarget
        );

    if(SUCCEEDED(hResult))
    {
        // Draw a pattern.
        ID2D1SolidColorBrush *pGridBrush = NULL;
        hResult = pCompatibleRenderTarget->CreateSolidColorBrush(
            D2D1::ColorF(D2D1::ColorF(0.93f, 0.94f, 0.96f, 1)),
            &pGridBrush
            );

        if(SUCCEEDED(hResult))
        {
            pCompatibleRenderTarget->BeginDraw();
            pCompatibleRenderTarget->FillRectangle(D2D1::RectF(0, 0, 10, 1), pGridBrush);
            pCompatibleRenderTarget->FillRectangle(D2D1::RectF(0, 0, 1, 10), pGridBrush);
            hResult = pCompatibleRenderTarget->EndDraw();

            if(hResult == D2DERR_RECREATE_TARGET)
            {
                // Discard all of the domain resources on this error.
                DiscardDeviceResources();
            }
            if(SUCCEEDED(hResult))
            {
                // Retrieve the bitmap from the render target.
                ID2D1Bitmap *pGridBitmap = NULL;
                hResult = pCompatibleRenderTarget->GetBitmap(&pGridBitmap);

                if(SUCCEEDED(hResult))
                {
                    // Create the bitmap brush.
                    hResult = pRenderTarget->CreateBitmapBrush(
                        pGridBitmap,
                        D2D1::BitmapBrushProperties(
                            D2D1_EXTEND_MODE_WRAP,
                            D2D1_EXTEND_MODE_WRAP),
                        ppBitmapBrush
                        );

                    pGridBitmap->Release();
                }
            }

            pGridBrush->Release();
        }

        pCompatibleRenderTarget->Release();
    }

	return hResult;
}

HRESULT CDxMainView::Draw2D()
{
	HRESULT hResult = CreateDeviceResources();
	if(SUCCEEDED(hResult))
	{
		m_pD2DRenderTarget->BeginDraw();
		m_pD2DRenderTarget->SetTransform(D2D1::IdentityMatrix());
		m_pD2DRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));
		
		D2D1_SIZE_F renderSize = m_pD2DRenderTarget->GetSize();
		m_pD2DRenderTarget->FillRectangle(D2D1::RectF(0, 0, renderSize.width, renderSize.height), m_pGridPatternBrush);

		DrawText0();
		hResult = m_pD2DRenderTarget->EndDraw();
	}
	if(FAILED(hResult))
	{
		DiscardDeviceResources();
	}

	return hResult;
}

VOID CDxMainView::DrawText0()
{
	RECT clientRect;
	GetClientRect(&clientRect);
	D2D1_POINT_2F origin = D2D1::Point2F(static_cast<FLOAT>(clientRect.left / m_dpiScaleX),
											static_cast<FLOAT>(clientRect.top / m_dpiScaleY));
	m_pD2DRenderTarget->DrawTextLayout(origin, m_pDWriteTextLayout0, m_pBlackBrush);
}

D2D1_POINT_2F CDxMainView::GetPageSize(IDWriteTextLayout* pTextLayout)
{
	D2D1_POINT_2F pageSize = {0};
	if(NULL != pTextLayout)
	{
		DWRITE_TEXT_METRICS textMetrics;
		if(SUCCEEDED(pTextLayout->GetMetrics(&textMetrics)))
		{
			float width = max(textMetrics.layoutWidth, textMetrics.left + textMetrics.width);
			float height = max(textMetrics.layoutHeight, textMetrics.height);
			pageSize.x = width;
			pageSize.y = height;
		}
	}

	return pageSize;
}