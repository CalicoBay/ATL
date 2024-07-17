#include "pch.h"
#include "D2DTab.h"

using namespace Microsoft::WRL;

CD2DTab::CD2DTab() :
	bAnimated(false),
	m_dpiScaleX(1.0),
	m_dpiScaleY(1.0),
	m_Angle(0),
	m_OriginX(0),
	m_OriginY(0)
{
}


CD2DTab::~CD2DTab()
{
}

HRESULT CD2DTab::Initialize(HWND hwndParent, LPCTSTR tszText /*= __nullptr*/)
{
	HRESULT hResult;
	if(__nullptr != tszText)
	{
		m_sText0.Format(_T("%s"), tszText);
	}

	HWND hwndThis = Create(hwndParent, CWindow::rcDefault);
	hResult = (0 != hwndThis) ? S_OK : E_FAIL;
	if(SUCCEEDED(hResult))
	{
		hResult = CreateDeviceIndependentResources();
	}

	return hResult;
}

HRESULT CD2DTab::CreateDeviceIndependentResources()
{
   HRESULT hResult = ::D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, m_ComPtrD2DFactory.GetAddressOf());

	m_dpiScaleX = ::GetDeviceCaps(GetDC(), LOGPIXELSX) / 96.0f;
	m_dpiScaleY = ::GetDeviceCaps(GetDC(), LOGPIXELSY) / 96.0f;

	return hResult;
}

HRESULT CD2DTab::CreateDeviceResources()
{
	HRESULT hResult = S_OK;
	RECT clientRect;
	GetClientRect(&clientRect);

	D2D1_SIZE_U size = D2D1::SizeU(clientRect.right - clientRect.left, clientRect.bottom - clientRect.top);
	//D2D1_SIZE_U size = D2D1::SizeU(816, 1056);
	//Create a Direct2D render target
	if(__nullptr == m_ComPtrD2DRenderTarget)
	{
      hResult = m_ComPtrD2DFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(),
														D2D1::HwndRenderTargetProperties(m_hWnd, size),
														&m_ComPtrD2DRenderTarget);
		//Create a Black Brush
		if(SUCCEEDED(hResult))
		{
			hResult = m_ComPtrD2DRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &m_ComPtrBlackBrush);
		}
		//Create the Grid Brush
		if(SUCCEEDED(hResult))
		{
			hResult = CreateGridPatternBrush(m_ComPtrD2DRenderTarget.Get(), &m_pGridPatternBrush);
		}
	}

	return hResult;
}

HRESULT CD2DTab::CreateGridPatternBrush(ID2D1RenderTarget* pRenderTarget, ID2D1BitmapBrush** ppBitmapBrush)
{
	HRESULT hResult = S_OK;

    // Create a compatible render target.
    ComPtr<ID2D1BitmapRenderTarget> pCompatibleRenderTarget;
    hResult = pRenderTarget->CreateCompatibleRenderTarget(
        D2D1::SizeF(10, 10),
        &pCompatibleRenderTarget
        );

    if(SUCCEEDED(hResult))
    {// Draw a pattern.
        ComPtr<ID2D1SolidColorBrush> pGridBrush;
        hResult = pCompatibleRenderTarget->CreateSolidColorBrush(
            D2D1::ColorF(D2D1::ColorF(0.93f, 0.94f, 0.96f, 1)),
            &pGridBrush
            );

        if(SUCCEEDED(hResult))
        {
            pCompatibleRenderTarget->BeginDraw();
            pCompatibleRenderTarget->FillRectangle(D2D1::RectF(0, 0, 10, 1), pGridBrush.Get());
            pCompatibleRenderTarget->FillRectangle(D2D1::RectF(0, 0, 1, 10), pGridBrush.Get());
            hResult = pCompatibleRenderTarget->EndDraw();

            if(hResult == D2DERR_RECREATE_TARGET)
            {//Discard all of the domain resources on this error.
               m_ComPtrD2DRenderTarget = __nullptr;
               m_ComPtrBlackBrush = __nullptr;
               m_pGridPatternBrush = __nullptr;
            }

            if(SUCCEEDED(hResult))
            {// Retrieve the bitmap from the render target.
                ComPtr<ID2D1Bitmap> pGridBitmap;
                hResult = pCompatibleRenderTarget->GetBitmap(&pGridBitmap);

                if(SUCCEEDED(hResult))
                {// Create the bitmap brush.
                    hResult = pRenderTarget->CreateBitmapBrush(pGridBitmap.Get(),
                        D2D1::BitmapBrushProperties(D2D1_EXTEND_MODE_WRAP, D2D1_EXTEND_MODE_WRAP), ppBitmapBrush);

                }
            }
        }
    }

	return hResult;
}

HRESULT CD2DTab::Draw2D()
{
	HRESULT hResult = CreateDeviceResources();
	if(SUCCEEDED(hResult))
	{
		m_ComPtrD2DRenderTarget->BeginDraw();
		m_ComPtrD2DRenderTarget->SetTransform(D2D1::IdentityMatrix());
		m_ComPtrD2DRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));
		
		D2D1_SIZE_F renderSize = m_ComPtrD2DRenderTarget->GetSize();
		m_ComPtrD2DRenderTarget->FillRectangle(D2D1::RectF(0, 0, renderSize.width, renderSize.height), m_pGridPatternBrush.Get());

		hResult = m_ComPtrD2DRenderTarget->EndDraw();
	}

   if(FAILED(hResult))
	{
      m_ComPtrD2DRenderTarget = __nullptr;
      m_ComPtrBlackBrush = __nullptr;
      m_pGridPatternBrush = __nullptr;
   }

	return hResult;
}


LRESULT CD2DTab::OnCreate(UINT /*nMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   UINT_PTR nID = SetTimer(0, 33, __nullptr);
   return 0;
}

LRESULT CD2DTab::OnPaint(UINT /*nMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	PAINTSTRUCT ps;
	BeginPaint(&ps);
		Draw2D();
	EndPaint(&ps);

	return 0;
}

LRESULT CD2DTab::OnSize(UINT /*nMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	D2D1_SIZE_U uSize;

	uSize.width = LOWORD(lParam);
	uSize.height = HIWORD(lParam);

	if(__nullptr != m_ComPtrD2DRenderTarget)
	{
		m_ComPtrD2DRenderTarget->Resize(uSize);
	}

	return 0;
}

