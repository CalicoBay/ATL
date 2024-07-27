#include "pch.h"
#include "D3DTab.h"

#pragma comment(linker, "/defaultlib:Dxgi.lib")
using namespace Microsoft::WRL;
float CD3DTab::t = 0.0f;

CD3DTab::CD3DTab() :
	m_bAnimated(true),
	m_bSuper(false),
   m_bCurrentFrontCCW(FALSE),
   m_bMultiSampling(FALSE),
   m_CurrentFillMode(D3D11_FILL_SOLID),
   m_CurrentCullMode(D3D11_CULL_BACK),
	m_D3D_Driver_Type(D3D_DRIVER_TYPE_NULL),
	m_D3D_Feature_Level(D3D_FEATURE_LEVEL(0)),//D3D_FEATURE_LEVEL_11_1
   m_hrInitializeIndependent(E_FAIL),
	m_hrInitializeDirect3D(E_FAIL)
{
   m_DXGI_SampleDesc.Count = 16U;
   m_DXGI_SampleDesc.Quality = unsigned(D3D11_STANDARD_MULTISAMPLE_PATTERN);
   m_bkgrndColor[0] = 0.0;
   m_bkgrndColor[1] = 0.0;
   m_bkgrndColor[2] = 0.0;
   m_bkgrndColor[3] = 0.0;
   m_PresentParams.DirtyRectsCount = 0;
	m_PresentParams.pDirtyRects = __nullptr;
	m_PresentParams.pScrollOffset = __nullptr;
	m_PresentParams.pScrollRect = __nullptr;
   m_csCurrentDir.Preallocate(MAX_PATH);
   ::GetCurrentDirectory(MAX_PATH, m_csCurrentDir.GetBuffer());
}


CD3DTab::~CD3DTab()
{
   //KillTimer(0);
   if(__nullptr != m_ComPtrImmediateContext) m_ComPtrImmediateContext->ClearState();
}

HRESULT CD3DTab::Initialize(HWND hwndParent, LPCTSTR tszText/* = __nullptr*/, bool bJustDirectX/* = false*/)
{
	HRESULT hResult;
   if(bJustDirectX)
   {
      hResult = S_OK;
   }
   else
   {
      HWND hwndThis = Create(hwndParent, CWindow::rcDefault);
      hResult = (0 != hwndThis) ? S_OK : E_FAIL;
   }

   if(__nullptr != tszText)
   {
      m_csInitText.Format(_T("%s"), tszText);
   }

   if(SUCCEEDED(hResult))
   {
      hResult = InitializeIndependent();
   }

   if(SUCCEEDED(hResult))
	{
		if(FAILED(m_hrInitializeDirect3D))
		{
			hResult = InitializeDirect3D();
		}
	}

	return hResult;
}

HRESULT CD3DTab::InitializeIndependent()
{
   m_hrInitializeIndependent = ::D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, m_ComPtrD2DFactory1.ReleaseAndGetAddressOf());
   if(SUCCEEDED(m_hrInitializeIndependent))
   {
      m_ComPtrIDWriteFactory7.ReleaseAndGetAddressOf();
      m_hrInitializeIndependent = ::DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory7), &m_ComPtrIDWriteFactory7);
   }
   if(SUCCEEDED(m_hrInitializeIndependent))
   {
      m_hrInitializeIndependent = m_ComPtrIDWriteFactory7->CreateTextFormat(L"Verdana", __nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
         DWRITE_FONT_STRETCH_NORMAL, 25, L"en-US", m_ComPtrDWriteTextFormat0.ReleaseAndGetAddressOf());
   }
   if(SUCCEEDED(m_hrInitializeIndependent))
   {
      m_ComPtrDWriteTextFormat0->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_JUSTIFIED);
      m_ComPtrDWriteTextFormat0->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
   }
   return m_hrInitializeIndependent;
}

HRESULT CD3DTab::InitializeDirect3D()
{
   HRESULT hr;
   RECT rc;
   GetClientRect(&rc);
   UINT width = rc.right - rc.left;
   UINT height = rc.bottom - rc.top;
   // This flag adds support for surfaces with a different color channel ordering
   // than the API default. It is required for compatibility with Direct2D.
      UINT createDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#ifdef _DEBUG
   createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

   D3D_DRIVER_TYPE driverTypes[] =
   {
      D3D_DRIVER_TYPE_HARDWARE,
      D3D_DRIVER_TYPE_WARP,
      D3D_DRIVER_TYPE_REFERENCE,
   };
   UINT numDriverTypes = ARRAYSIZE( driverTypes );

   D3D_FEATURE_LEVEL featureLevels[] =
   {
      D3D_FEATURE_LEVEL_11_1,
      D3D_FEATURE_LEVEL_11_0,
      D3D_FEATURE_LEVEL_10_1,
      D3D_FEATURE_LEVEL_10_0,
   };
   UINT numFeatureLevels = ARRAYSIZE( featureLevels );
   ComPtr<ID3D11Device> ComPtrD3DDevice;
   ComPtr<ID3D11DeviceContext> ComPtrImmediateContext;
   for(UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; ++driverTypeIndex)
   {
      m_D3D_Driver_Type = driverTypes[driverTypeIndex];
      m_hrInitializeDirect3D = ::D3D11CreateDevice(__nullptr, m_D3D_Driver_Type, __nullptr, createDeviceFlags,
									      featureLevels, numFeatureLevels, D3D11_SDK_VERSION,
										      &ComPtrD3DDevice, &m_D3D_Feature_Level, &ComPtrImmediateContext);
      if(SUCCEEDED(m_hrInitializeDirect3D))
      {
         m_ComPtrD3DDevice.ReleaseAndGetAddressOf();
         m_hrInitializeDirect3D = ComPtrD3DDevice.As(&m_ComPtrD3DDevice);
         if(SUCCEEDED(m_hrInitializeDirect3D))
         {
            m_ComPtrImmediateContext.ReleaseAndGetAddressOf();
            m_hrInitializeDirect3D = ComPtrImmediateContext.As(&m_ComPtrImmediateContext);
         }
         break;
      }
   }
    
   if(SUCCEEDED(m_hrInitializeDirect3D))
   {    // Create a default rasterizer state
      m_hrInitializeDirect3D = m_ComPtrD3DDevice->CreateRasterizerState1(&CD3D11_RASTERIZER_DESC1(D3D11_DEFAULT), m_ComPtrRastCurrent.ReleaseAndGetAddressOf());
      if(SUCCEEDED(m_hrInitializeDirect3D))
      {
         m_ComPtrImmediateContext->RSSetState(m_ComPtrRastCurrent.Get());
      }
   }

   if (SUCCEEDED(m_hrInitializeDirect3D))
   {
      UINT uiQuality = 0;
      while (0U == uiQuality && 0U < m_DXGI_SampleDesc.Count)
      {
         m_hrInitializeDirect3D = m_ComPtrD3DDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_B8G8R8A8_UNORM, m_DXGI_SampleDesc.Count, &uiQuality);
         if (SUCCEEDED(m_hrInitializeDirect3D))
         {
            if (0 == uiQuality)
            {
               m_DXGI_SampleDesc.Count >>= 1;
            }
         }
         else
         {
            break;
         }
      }
      if (0U == m_DXGI_SampleDesc.Count)
      {
         m_DXGI_SampleDesc.Count = 1U;
      }
   }

   if(SUCCEEDED(m_hrInitializeDirect3D))
   {
      D3D11_TEXTURE2D_DESC tex2DDesc = {};
      tex2DDesc.ArraySize = 1;
      tex2DDesc.BindFlags = D3D11_BIND_RENDER_TARGET;// | D3D11_BIND_SHADER_RESOURCE;
      tex2DDesc.CPUAccessFlags = 0;
      tex2DDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
      tex2DDesc.Height = 512;
      tex2DDesc.MipLevels = 1;
      tex2DDesc.MiscFlags = 0;
      tex2DDesc.SampleDesc.Count = m_DXGI_SampleDesc.Count;
      tex2DDesc.SampleDesc.Quality = m_DXGI_SampleDesc.Quality;
      tex2DDesc.Usage = D3D11_USAGE_DEFAULT;
      tex2DDesc.Width = 512;
      hr = m_ComPtrD3DDevice->CreateTexture2D(&tex2DDesc, __nullptr, m_ComPtrID3D11Texture2DOffscreen.ReleaseAndGetAddressOf());
      if(SUCCEEDED(hr))
      {
         ComPtr<IDXGISurface2> ComPtrIDXGISurface2;
         hr = m_ComPtrID3D11Texture2DOffscreen.As(&ComPtrIDXGISurface2);
         if(SUCCEEDED(hr))
         { 
            D2D1_RENDER_TARGET_PROPERTIES rtProps = D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_DEFAULT, D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED), 96, 96);
            hr = m_ComPtrD2DFactory1->CreateDxgiSurfaceRenderTarget(ComPtrIDXGISurface2.Get(), rtProps, m_ComPtrDXGISurfaceRenderTarget.ReleaseAndGetAddressOf());
            if(SUCCEEDED(hr))
            {
               hr = m_ComPtrDXGISurfaceRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Yellow, 0.5f), m_ComPtrTextBrush.ReleaseAndGetAddressOf());
            }
         }
      }
   }
   return m_hrInitializeDirect3D;
}


LRESULT CD3DTab::OnContextMenu(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
   LRESULT lResult = 0;
   POINT ptScreen;
   UINT idBegin = IDS_3DCONTEXT0, idEnd = IDS_3DCONTEXT9 + 1UL;
   CStringW csItem;
   ptScreen.x = GET_X_LPARAM(lParam);
   ptScreen.y = GET_Y_LPARAM(lParam);
   if(-1L == ptScreen.x && -1L == ptScreen.y)
   {// App Key/Context Key / Shift F10
      GetCaretPos(&ptScreen);
   }
   HMENU hPopup = ::CreatePopupMenu();
   if(__nullptr != hPopup)
   {
      MENUITEMINFO menuItemInfo = {0};
      menuItemInfo.cbSize = sizeof(MENUITEMINFO);
      menuItemInfo.fMask = MIIM_TYPE | MIIM_ID;
      menuItemInfo.fType = MFT_STRING;
      for(UINT id = idBegin; id < idEnd; ++id)
      {
         menuItemInfo.wID = id;
         csItem.Format(id);
         menuItemInfo.dwTypeData = csItem.GetBuffer();
         menuItemInfo.cch = csItem.GetLength();
         ::InsertMenuItem(hPopup, menuItemInfo.wID, FALSE, &menuItemInfo);
         csItem.Empty();
      }
      int idCmd = ::TrackPopupMenuEx(hPopup, TPM_RIGHTALIGN | TPM_TOPALIGN | TPM_RETURNCMD, ptScreen.x, ptScreen.y, m_hWnd, __nullptr);
      ::DestroyMenu(hPopup);
      if(0L != idCmd && IDS_3DCONTEXT9 >= idCmd)
      {
         D3D11_RASTERIZER_DESC1 rastDesc1;
         m_ComPtrRastCurrent->GetDesc1(&rastDesc1);
         switch(idCmd)
         {
         case IDS_3DCONTEXT0:// Toggle Wireframe
            if(D3D11_FILL_SOLID == m_CurrentFillMode)
            {
               rastDesc1.FillMode = D3D11_FILL_WIREFRAME;
               m_CurrentFillMode = D3D11_FILL_WIREFRAME;
            }
            else
            {
               rastDesc1.FillMode = D3D11_FILL_SOLID;
               m_CurrentFillMode = D3D11_FILL_SOLID;
            }
            if(SUCCEEDED(m_ComPtrD3DDevice->CreateRasterizerState1(&rastDesc1, m_ComPtrRastCurrent.ReleaseAndGetAddressOf())))
            {
               m_ComPtrImmediateContext->RSSetState(m_ComPtrRastCurrent.Get());
            }
            break;
         case IDS_3DCONTEXT1:// Cull Front
            rastDesc1.CullMode = D3D11_CULL_FRONT;
            if(SUCCEEDED(m_ComPtrD3DDevice->CreateRasterizerState1(&rastDesc1, m_ComPtrRastCurrent.ReleaseAndGetAddressOf())))
            {
               m_ComPtrImmediateContext->RSSetState(m_ComPtrRastCurrent.Get());
               m_CurrentCullMode = D3D11_CULL_FRONT;
            }
            break;
         case IDS_3DCONTEXT2:// Cull Back
            rastDesc1.CullMode = D3D11_CULL_BACK;
            if(SUCCEEDED(m_ComPtrD3DDevice->CreateRasterizerState1(&rastDesc1, m_ComPtrRastCurrent.ReleaseAndGetAddressOf())))
            {
               m_ComPtrImmediateContext->RSSetState(m_ComPtrRastCurrent.Get());
               m_CurrentCullMode = D3D11_CULL_BACK;
            }
            break;
         case IDS_3DCONTEXT3:// Cull None
            rastDesc1.CullMode = D3D11_CULL_NONE;
            if(SUCCEEDED(m_ComPtrD3DDevice->CreateRasterizerState1(&rastDesc1, m_ComPtrRastCurrent.ReleaseAndGetAddressOf())))
            {
               m_ComPtrImmediateContext->RSSetState(m_ComPtrRastCurrent.Get());
               m_CurrentCullMode = D3D11_CULL_NONE;
            }
            break;
         case IDS_3DCONTEXT4:// Fronts CCW
            rastDesc1.FrontCounterClockwise = !m_bCurrentFrontCCW;
            if(SUCCEEDED(m_ComPtrD3DDevice->CreateRasterizerState1(&rastDesc1, m_ComPtrRastCurrent.ReleaseAndGetAddressOf())))
            {
               m_ComPtrImmediateContext->RSSetState(m_ComPtrRastCurrent.Get());
               m_bCurrentFrontCCW = rastDesc1.FrontCounterClockwise;
            }
            break;
         case IDS_3DCONTEXT5:// Toggle MultiSampling
            rastDesc1.MultisampleEnable = !m_bMultiSampling;
            if(SUCCEEDED(m_ComPtrD3DDevice->CreateRasterizerState1(&rastDesc1, m_ComPtrRastCurrent.ReleaseAndGetAddressOf())))
            {
               m_ComPtrImmediateContext->RSSetState(m_ComPtrRastCurrent.Get());
               m_bMultiSampling = rastDesc1.MultisampleEnable;
            }
            break;
         case IDS_3DCONTEXT6:// Defaults
         default:
            if(SUCCEEDED(m_ComPtrD3DDevice->CreateRasterizerState1(&CD3D11_RASTERIZER_DESC1(D3D11_DEFAULT), m_ComPtrRastCurrent.ReleaseAndGetAddressOf())))
            {
               m_ComPtrImmediateContext->RSSetState(m_ComPtrRastCurrent.Get());
               m_bCurrentFrontCCW = FALSE;
               m_bMultiSampling = FALSE;
               m_CurrentFillMode = D3D11_FILL_SOLID;
               m_CurrentCullMode = D3D11_CULL_BACK;
            }
            break;
         }
      }
   }
   return lResult;
}

LRESULT CD3DTab::OnCreate(UINT /*nMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   UINT_PTR nID = SetTimer(0, 15, __nullptr);
   return 0;
}

LRESULT CD3DTab::OnDisplayChange(UINT /*nMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	InvalidateRect(__nullptr, FALSE);
	return 0;
}

LRESULT CD3DTab::OnPaint(UINT /*nMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	Render();
	ValidateRect(__nullptr);
	return 0;
}

LRESULT CD3DTab::OnSize(UINT /*nMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
   if((LPARAM)__nullptr != lParam)
	{//We're not just initializing
		UINT uiWidth = UINT(LOWORD(lParam));
		UINT uiHeight = UINT(HIWORD(lParam));
		RecreateSizedResources(uiWidth, uiHeight);
	}
	
	return 0;
}

VOID CD3DTab::Clear()
{
    // Just clear the backbuffer
   m_ComPtrImmediateContext->ClearRenderTargetView(m_ComPtrRenderTargetView.Get(), m_bkgrndColor);
    m_ComPtrSwapChain->Present1(1, 0, &m_PresentParams);
}

VOID CD3DTab::Render()
{
   m_csText0.Format(L"t = %f", t /*+= 1.0f*/);
   // Just clear the backbuffer
   m_ComPtrImmediateContext->ClearRenderTargetView(m_ComPtrRenderTargetView.Get(), m_bkgrndColor);
   // Try out some text
   if(__nullptr != m_ComPtrD2DRenderTarget)
   {
      m_ComPtrD2DRenderTarget->BeginDraw();
      D2D1_SIZE_F rtSize = m_ComPtrD2DRenderTarget->GetSize();
      m_ComPtrD2DRenderTarget->DrawText(m_csText0, m_csText0.GetLength(), m_ComPtrDWriteTextFormat0.Get(), D2D1::RectF(0.f, 0.f, rtSize.width, rtSize.height), m_ComPtrWhiteBrush.Get());
      m_ComPtrD2DRenderTarget->EndDraw();
   }
   m_ComPtrSwapChain->Present1(1, 0, &m_PresentParams);
}

HRESULT CD3DTab::RecreateSizedResources(UINT uiWidth, UINT uiHeight)
{
	HRESULT hResult = m_hrInitializeDirect3D;
   if(FAILED(hResult))
   {
      m_hrInitializeDirect3D = Initialize(__nullptr, __nullptr, true);
      hResult = m_hrInitializeDirect3D;
   }

	if(SUCCEEDED(hResult))
	{
		ID3D11RenderTargetView* nullViews[] = {__nullptr};
		m_ComPtrImmediateContext->OMSetRenderTargets(_ARRAYSIZE(nullViews), nullViews, __nullptr);
      m_ComPtrD2DRenderTarget = __nullptr;
      m_ComPtrRenderTargetView = __nullptr;
		m_ComPtrDepthStencilView = __nullptr;
		m_ComPtrImmediateContext->Flush();

		if(__nullptr != m_ComPtrSwapChain)
		{//Resize the SwapChain if it exists
			hResult = m_ComPtrSwapChain->ResizeBuffers(2U, uiWidth, uiHeight, DXGI_FORMAT_B8G8R8A8_UNORM, 0U);
		}
      else
      {//Create a SwapChain via a IDXGIFactory1 and IDXGIFactory2
         DXGI_SWAP_CHAIN_DESC1 sd = {};
         sd.Width = uiWidth;
         sd.Height = uiHeight;
         sd.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
         sd.Stereo = false;
         sd.SampleDesc.Count = m_DXGI_SampleDesc.Count;
         sd.SampleDesc.Quality = m_DXGI_SampleDesc.Quality;
         sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
         sd.BufferCount = 2;
         sd.Scaling = DXGI_SCALING_STRETCH;//DXGI_SCALING_NONE caused CreateSwapChainforHwnd to return DXGI_ERROR_INVALID_CALL(0x887a0001)
         sd.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
         sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;//DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL; //DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL; // All Windows Store apps must use this SwapEffect.
         sd.Flags = 0;

         ComPtr<IDXGIDevice2> pDXGIDevice2;
         ComPtr<IDXGIAdapter> pDXGIAdapter;
         ComPtr<IDXGIFactory2> pIDXGIFactory2;
         hResult = m_ComPtrD3DDevice.As(&pDXGIDevice2);
         if(SUCCEEDED(hResult))
         {
            hResult = pDXGIDevice2->GetParent(__uuidof(IDXGIAdapter), &pDXGIAdapter);
            if(SUCCEEDED(hResult))
            {
               hResult = pDXGIAdapter->GetParent(__uuidof(IDXGIFactory2), &pIDXGIFactory2);
               if(SUCCEEDED(hResult))
               {
                  hResult = pIDXGIFactory2->CreateSwapChainForHwnd(m_ComPtrD3DDevice.Get(), m_hWnd, &sd, __nullptr, __nullptr, &m_ComPtrSwapChain);
                  if(SUCCEEDED(hResult))
                  {  // Ensure that DXGI does not queue more than one frame at a time. This both reduces latency and
                     // ensures that the application will only render after each VSync, minimizing power consumption.
                     // Frame latency is the number of frames that are allowed to be stored in a queue
                     // before submission for rendering.Latency is often used to control how the CPU
                     // chooses between responding to user input and frames that are in the render queue.
                     // It is often beneficial for applications that have no user input(for example, video playback)
                     // to queue more than 3 frames of data.
                     hResult = pDXGIDevice2->SetMaximumFrameLatency(1);
                  }
               }
            }
         }
      }
		if(!m_bSuper)
		{//We should probably be a pure virtual base but for now we're not
			//so lets make that possible.
			if(SUCCEEDED(hResult))
			{
				// Create a render target view
				ComPtr<ID3D11Texture2D> pBackBuffer;
				hResult = m_ComPtrSwapChain->GetBuffer(0, __uuidof( ID3D11Texture2D ), &pBackBuffer);
				if(FAILED(hResult))
				{
					return hResult;
				}

				hResult = m_ComPtrD3DDevice->CreateRenderTargetView(pBackBuffer.Get(), __nullptr, &m_ComPtrRenderTargetView);
				if(FAILED(hResult))
				{
					return hResult;
				}

            if(SUCCEEDED(hResult))
            {
               ComPtr<IDXGISurface2> pIDXGISurface2;
               hResult = pBackBuffer.As(&pIDXGISurface2);
               if(SUCCEEDED(hResult))
               {
                  D2D1_RENDER_TARGET_PROPERTIES rtProps = D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_DEFAULT, D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED), 96, 96);
                  hResult = m_ComPtrD2DFactory1->CreateDxgiSurfaceRenderTarget(pIDXGISurface2.Get(), rtProps, m_ComPtrD2DRenderTarget.ReleaseAndGetAddressOf());
                  if(SUCCEEDED(hResult))
                  {
                     if(__nullptr == m_ComPtrWhiteBrush)
                     {
                        hResult = m_ComPtrD2DRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White, 0.4f), m_ComPtrWhiteBrush.ReleaseAndGetAddressOf());
                     }
                  }
               }
            }
         
            if(FAILED(hResult))
            {
               return hResult;
            }

            m_ComPtrImmediateContext->OMSetRenderTargets(1, m_ComPtrRenderTargetView.GetAddressOf(), __nullptr);

				// Setup the viewport
				D3D11_VIEWPORT vp;
				vp.Width = (FLOAT)uiWidth;
				vp.Height = (FLOAT)uiHeight;
				vp.MinDepth = 0.0f;
				vp.MaxDepth = 1.0f;
				vp.TopLeftX = 0;
				vp.TopLeftY = 0;
				m_ComPtrImmediateContext->RSSetViewports( 1, &vp );
			}
		}
	}
	return hResult;
}