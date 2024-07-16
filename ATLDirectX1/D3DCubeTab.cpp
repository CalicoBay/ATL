#include "stdafx.h"
#include "D3DCubeTab.h"

using namespace std;

CD3DCubeTab::CD3DCubeTab(void) :
	CD3DTab(),
	m_indexCount(0),
	m_Orientation(1U),//DisplayOrientations::Landscape
	m_hrInitializeCube(E_FAIL)
{
	SetSuper();
}


CD3DCubeTab::~CD3DCubeTab(void)
{
}

HRESULT CD3DCubeTab::Initialize(HWND hwndParent, LPCTSTR tszText, bool bJustDirectX/* = false*/)
{
	m_hrInitializeCube = __super::Initialize(hwndParent, tszText, bJustDirectX);
	if(SUCCEEDED(m_hrInitializeCube))
	{
		PUINT8 pData = __nullptr;
		size_t uiLength = 0;
      //CString csShader;
      //csShader.Format(_T("%s\\SimpleVertexShader.cso"), m_csCurrentDir);
		//m_hrInitializeCube = ReadData(csShader, pData, uiLength);
      m_hrInitializeCube = DX::LoadShaderFromResource(MAKEINTRESOURCE(IDR_VERTEX_SHADER), pData, uiLength);
		if(SUCCEEDED(m_hrInitializeCube))
		{
			m_hrInitializeCube = m_ComPtrD3DDevice->CreateVertexShader(pData, uiLength, __nullptr, &m_ComPtrVertexShader);
		}
		if(SUCCEEDED(m_hrInitializeCube))
		{
			const D3D11_INPUT_ELEMENT_DESC vertexDesc[] = 
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "COLOR",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			};

			m_hrInitializeCube = m_ComPtrD3DDevice->CreateInputLayout(vertexDesc, ARRAYSIZE(vertexDesc), pData, uiLength, &m_ComPtrInputLayout);
			//delete[] pData;
			pData = __nullptr;
			uiLength = 0;
		}
		if(SUCCEEDED(m_hrInitializeCube))
		{
         //csShader.Format(_T("%s\\SimplePixelShader.cso"), m_csCurrentDir);
         //m_hrInitializeCube = ReadData(csShader, pData, uiLength);
         m_hrInitializeCube = DX::LoadShaderFromResource(MAKEINTRESOURCE(IDR_PIXEL_SHADER), pData, uiLength);
      }
		if(SUCCEEDED(m_hrInitializeCube))
		{
			m_hrInitializeCube = m_ComPtrD3DDevice->CreatePixelShader(pData, uiLength, __nullptr, &m_ComPtrPixelShader);
			//delete[] pData;
			pData = __nullptr;
			uiLength = 0;
		}
		if(SUCCEEDED(m_hrInitializeCube))
		{
			CD3D11_BUFFER_DESC constantBufferDesc(sizeof(ModelViewProjectionConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
			m_hrInitializeCube = m_ComPtrD3DDevice->CreateBuffer(&constantBufferDesc, __nullptr, &m_ComPtrConstantBuffer);
		}
		if(SUCCEEDED(m_hrInitializeCube))
		{
			//VertexPositionColor cubeVertices[] = 
			//{
			//	{XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT3(0.0f, 0.0f, 0.0f)},
			//	{XMFLOAT3(-0.5f, -0.5f,  0.5f), XMFLOAT3(0.0f, 0.0f, 1.0f)},
			//	{XMFLOAT3(-0.5f,  0.5f, -0.5f), XMFLOAT3(0.0f, 1.0f, 0.0f)},
			//	{XMFLOAT3(-0.5f,  0.5f,  0.5f), XMFLOAT3(0.0f, 1.0f, 1.0f)},
			//	{XMFLOAT3( 0.5f, -0.5f, -0.5f), XMFLOAT3(1.0f, 0.0f, 0.0f)},
			//	{XMFLOAT3( 0.5f, -0.5f,  0.5f), XMFLOAT3(1.0f, 0.0f, 1.0f)},
			//	{XMFLOAT3( 0.5f,  0.5f, -0.5f), XMFLOAT3(1.0f, 1.0f, 0.0f)},
			//	{XMFLOAT3( 0.5f,  0.5f,  0.5f), XMFLOAT3(1.0f, 1.0f, 1.0f)},
			//};

         VertexPositionColor altCubeVertices[] =
         {  //+Z
            {XMFLOAT3(-0.5f, 0.5f, 0.5f), XMFLOAT3(0.25f, 0.25f, 1.0f)},
            {XMFLOAT3(-0.5f, -0.5f, 0.5f), XMFLOAT3(0.25f, 0.25f, 1.0f)},
            {XMFLOAT3(0.5f, 0.5f, 0.5f), XMFLOAT3(0.25f, 0.25f, 1.0f)},
            {XMFLOAT3(0.5f, -0.5f, 0.5f), XMFLOAT3(0.25f, 0.25f, 1.0f)},
            //-X
            {XMFLOAT3(-0.5f, 0.5f, -0.5f), XMFLOAT3(0.5f, 0.0f, 0.0f)},
            {XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT3(0.5f, 0.0f, 0.0f)},
            {XMFLOAT3(-0.5f, 0.5f, 0.5f), XMFLOAT3(0.5f, 0.0f, 0.0f)},
            {XMFLOAT3(-0.5f, -0.5f, 0.5f), XMFLOAT3(0.5f, 0.0f, 0.0f)},
            //+Y
            {XMFLOAT3(0.5f, 0.5f, -0.5f), XMFLOAT3(0.25f, 1.0f, 0.25f)},
            {XMFLOAT3(-0.5f, 0.5f, -0.5f), XMFLOAT3(0.25f, 1.0f, 0.25f)},
            {XMFLOAT3(0.5f, 0.5f, 0.5f), XMFLOAT3(0.25f, 1.0f, 0.25f)},
            {XMFLOAT3(-0.5f, 0.5f, 0.5f), XMFLOAT3(0.25f, 1.0f, 0.25f)},
            //+X
            {XMFLOAT3(0.5f, 0.5f, 0.5f), XMFLOAT3(1.0f, 0.15f, 0.25f)},
            {XMFLOAT3(0.5f, -0.5f, 0.5f), XMFLOAT3(1.0f, 0.15f, 0.25f)},
            {XMFLOAT3(0.5f, 0.5f, -0.5f), XMFLOAT3(1.0f, 0.15f, 0.25f)},
            {XMFLOAT3(0.5f, -0.5f, -0.5f), XMFLOAT3(1.0f, 0.15f, 0.25f)},
            //-Y
            {XMFLOAT3(0.5f, -0.5f, 0.5f), XMFLOAT3(0.0f, 0.5f, 0.0f)},
            {XMFLOAT3(-0.5f, -0.5f, 0.5f), XMFLOAT3(0.0f, 0.5f, 0.0f)},
            {XMFLOAT3(0.5f, -0.5f, -0.5f), XMFLOAT3(0.0f, 0.5f, 0.0f)},
            {XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT3(0.0f, 0.5f, 0.0f)},
            //-Z
            {XMFLOAT3(0.5f, 0.5f, -0.5f), XMFLOAT3(0.0f, 0.0f, 0.5f)},
            {XMFLOAT3(0.5f, -0.5f, -0.5f), XMFLOAT3(0.0f, 0.0f, 0.5f)},
            {XMFLOAT3(-0.5f, 0.5f, -0.5f), XMFLOAT3(0.0f, 0.0f, 0.5f)},
            {XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT3(0.0f, 0.0f, 0.5f)},
         };
         
         D3D11_SUBRESOURCE_DATA vertexBufferData = {0};
			vertexBufferData.pSysMem = altCubeVertices;
			vertexBufferData.SysMemPitch = 0;
			vertexBufferData.SysMemSlicePitch = 0;
			CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(altCubeVertices), D3D11_BIND_VERTEX_BUFFER);
			m_hrInitializeCube = m_ComPtrD3DDevice->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &m_ComPtrVertexBuffer);
		}
		if(SUCCEEDED(m_hrInitializeCube))
		{
			//unsigned short cubeIndices[] = 
			//{
			//	0,2,1, // -x
			//	1,2,3,

			//	4,5,6, // +x
			//	5,7,6,

			//	0,1,5, // -y
			//	0,5,4,

			//	2,6,7, // +y
			//	2,7,3,

			//	0,4,6, // -z
			//	0,6,2,

			//	1,3,7, // +z
			//	1,7,5,
			//};

         unsigned short altCubeIndices[] =
         {
            0, 1, 2, //+Z
            1, 3, 2,

            4, 5, 6, //-X
            5, 7, 6,

            8, 9, 10, //+Y
            9, 11, 10,

            12, 13, 14, //+X
            13, 15, 14,

            16, 17, 18, //-Y
            17, 19, 18,

            20, 21, 22, //-Z
            21, 23, 22,
         };

         m_indexCount = _ARRAYSIZE(altCubeIndices);

			D3D11_SUBRESOURCE_DATA indexBufferData = {0};
			indexBufferData.pSysMem = altCubeIndices;
			indexBufferData.SysMemPitch = 0;
			indexBufferData.SysMemSlicePitch = 0;
			CD3D11_BUFFER_DESC indexBufferDesc(sizeof(altCubeIndices), D3D11_BIND_INDEX_BUFFER);
			m_hrInitializeCube = m_ComPtrD3DDevice->CreateBuffer(&indexBufferDesc, &indexBufferData, &m_ComPtrIndexBuffer);
		}
	}

   XMVECTOR eye = XMVectorSet(0.0f, 0.7f, 1.5f, 0.0f);
   XMVECTOR at = XMVectorSet(0.0f, -0.1f, 0.0f, 0.0f);
   XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

   XMStoreFloat4x4(&m_constantBufferData.view, XMMatrixTranspose(XMMatrixLookAtLH(eye, at, up)));
   
   return m_hrInitializeCube;
}

//HRESULT CD3DCubeTab::LoadShaderResource(PCTSTR ptszResource, PCHAR& pBuffer, size_t& uiCount)
//{
//   HRESULT hr = E_NOT_SET;
//   // Locate the resource handle in our DLL.
//   HRSRC hResource = ::FindResource(GetModuleHINSTANCE(), ptszResource, RT_RCDATA);
//   if(__nullptr != hResource)
//   {
//      // Load the resource.
//      HGLOBAL hResourceData = ::LoadResource(GetModuleHINSTANCE(), hResource);
//      if(__nullptr != hResourceData)
//      {
//         // Lock it to get a system memory pointer.
//         pBuffer = (PCHAR)::LockResource(hResourceData);
//         if(__nullptr != pBuffer)
//         {
//            // Calculate the size.
//            uiCount = ::SizeofResource(GetModuleHINSTANCE(), hResource);
//            if(0 < uiCount)
//            {
//               hr = S_OK;
//            }
//         }
//      }
//   }
//   return hr;
//}

//HRESULT CD3DCubeTab::ReadData(LPCTSTR csFile, PCHAR& pBuffer, size_t& uiCount)
//{
//	HRESULT hResult = S_OK;
//	ios_base::openmode openMode = ios_base::in|ios_base::binary;
//	ifstream fileToRead(csFile, openMode);
//	if(fileToRead.is_open())
//	{
//		fileToRead.seekg(0, ios_base::end);
//		uiCount = fileToRead.tellg();
//		fileToRead.seekg(0, ios_base::beg);
//		pBuffer = new(nothrow)CHAR[uiCount];
//		if(__nullptr == pBuffer)
//		{
//			hResult = E_OUTOFMEMORY;
//		}
//		else
//		{
//			UINT i = 0;
//			while(!fileToRead.eof())
//			{
//				fileToRead.read((pBuffer + i), 0x100);
//				if(pBuffer[i] == 0 && fileToRead.eof())
//				{
//					break;
//				}
//				i += 0x100;
//			}
//		}
//
//		fileToRead.close();
//	}
//	else
//	{
//		hResult = E_INVALIDARG;
//	}
//
//	return hResult;
//}

VOID CD3DCubeTab::Render()
{
	//static float t = 0.0f;
	if(D3D_DRIVER_TYPE_REFERENCE == m_D3D_Driver_Type)
	{
		t += float(XM_PI * 0.0125f);
	}
	else
	{
		static DWORD dwTimeStart = 0;
      static DWORD dwLastTick = 0;
		DWORD dwTimeCur = ::GetTickCount();
		if(0 == dwTimeStart)
		{
			dwTimeStart = dwLastTick = dwTimeCur;
		}
      DWORD dwDiff = dwTimeCur - dwLastTick;
      dwLastTick = dwTimeCur;
      t = (dwTimeCur - dwTimeStart)/ 1000.0f;
      m_csText0.Format(L"Frame Rate = %f", 1000.f/dwDiff);
   }
   //XMStoreFloat4x4(&m_constantBufferData.model, XMMatrixTranspose(XMMatrixRotationX(t)));
   //XMStoreFloat4x4(&m_constantBufferData.model, XMMatrixTranspose(XMMatrixRotationY(t)));
   //XMStoreFloat4x4(&m_constantBufferData.model, XMMatrixTranspose(XMMatrixRotationZ(t)));
   XMStoreFloat4x4(&m_constantBufferData.model, XMMatrixTranspose(XMMatrixRotationRollPitchYaw(t, t, t)));
   //float ClearColor[4] = {m_bkgrndColor.x, m_bkgrndColor.y, m_bkgrndColor.z, m_bkgrndColor.w}; //red,green,blue,alpha
   m_ComPtrImmediateContext->ClearRenderTargetView(m_ComPtrRenderTargetView.Get(), m_bkgrndColor);
	m_ComPtrImmediateContext->ClearDepthStencilView(m_ComPtrDepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	m_ComPtrImmediateContext->OMSetRenderTargets(1, m_ComPtrRenderTargetView.GetAddressOf(), m_ComPtrDepthStencilView.Get());
	m_ComPtrImmediateContext->UpdateSubresource(m_ComPtrConstantBuffer.Get(), 0U, __nullptr, &m_constantBufferData, 0U, 0U);
	UINT uiStride = sizeof(VertexPositionColor);
	UINT uiOffset = 0U;
	m_ComPtrImmediateContext->IASetVertexBuffers(0U, 1U, m_ComPtrVertexBuffer.GetAddressOf(), &uiStride, &uiOffset);
	m_ComPtrImmediateContext->IASetIndexBuffer(m_ComPtrIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0U);
	m_ComPtrImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_ComPtrImmediateContext->IASetInputLayout(m_ComPtrInputLayout.Get());
	m_ComPtrImmediateContext->VSSetShader(m_ComPtrVertexShader.Get(), __nullptr, 0U);
	m_ComPtrImmediateContext->VSSetConstantBuffers(0U, 1U, m_ComPtrConstantBuffer.GetAddressOf());
	m_ComPtrImmediateContext->PSSetShader(m_ComPtrPixelShader.Get(), __nullptr, 0U);
	m_ComPtrImmediateContext->DrawIndexed(m_indexCount, 0U, 0);
   // Try out some text
   if(__nullptr != m_ComPtrD2DRenderTarget)
   {
      m_ComPtrD2DRenderTarget->BeginDraw();
      D2D1_SIZE_F rtSize = m_ComPtrD2DRenderTarget->GetSize();
      m_ComPtrD2DRenderTarget->DrawText(m_csText0, m_csText0.GetLength(), m_ComPtrDWriteTextFormat0.Get(), D2D1::RectF(0.f, 0.f, rtSize.width, rtSize.height), m_ComPtrTextBrush.Get());
      m_ComPtrD2DRenderTarget->EndDraw();
   }
   m_ComPtrSwapChain->Present1(1, 0, &m_PresentParams);
	m_ComPtrImmediateContext->DiscardView(m_ComPtrRenderTargetView.Get());
	m_ComPtrImmediateContext->DiscardView(m_ComPtrDepthStencilView.Get());
}

HRESULT CD3DCubeTab::RecreateSizedResources(UINT uiWidth, UINT uiHeight)
{
	HRESULT hResult = m_hrInitializeCube;
	if(SUCCEEDED(hResult))
	{
		hResult = __super::RecreateSizedResources(uiWidth, uiHeight);
	}
   else
   {
      m_hrInitializeCube = Initialize(__nullptr, __nullptr, true);
      hResult = m_hrInitializeCube;
   }

	if(SUCCEEDED(hResult))
	{
		// Set the proper orientation for the swap chain, and generate the
		// 3D matrix transformation for rendering to the rotated swap chain.
		DXGI_MODE_ROTATION rotation = DXGI_MODE_ROTATION_UNSPECIFIED;
		switch (m_Orientation)
		{
		case 1U://DisplayOrientations::Landscape:
				rotation = DXGI_MODE_ROTATION_IDENTITY;
				m_OrientationTransform3D = XMFLOAT4X4( // 0-degree Z-rotation
					1.0f, 0.0f, 0.0f, 0.0f,
					0.0f, 1.0f, 0.0f, 0.0f,
					0.0f, 0.0f, 1.0f, 0.0f,
					0.0f, 0.0f, 0.0f, 1.0f
					);
				break;

		case 2U://DisplayOrientations::Portrait:
				rotation = DXGI_MODE_ROTATION_ROTATE270;
				m_OrientationTransform3D = XMFLOAT4X4( // 90-degree Z-rotation
					0.0f, 1.0f, 0.0f, 0.0f,
					-1.0f, 0.0f, 0.0f, 0.0f,
					0.0f, 0.0f, 1.0f, 0.0f,
					0.0f, 0.0f, 0.0f, 1.0f
					);
				break;

		case 4U://DisplayOrientations::LandscapeFlipped:
				rotation = DXGI_MODE_ROTATION_ROTATE180;
				m_OrientationTransform3D = XMFLOAT4X4( // 180-degree Z-rotation
					-1.0f, 0.0f, 0.0f, 0.0f,
					0.0f, -1.0f, 0.0f, 0.0f,
					0.0f, 0.0f, 1.0f, 0.0f,
					0.0f, 0.0f, 0.0f, 1.0f
					);
				break;

		case 8U://DisplayOrientations::PortraitFlipped:
				rotation = DXGI_MODE_ROTATION_ROTATE90;
				m_OrientationTransform3D = XMFLOAT4X4( // 270-degree Z-rotation
					0.0f, -1.0f, 0.0f, 0.0f,
					1.0f, 0.0f, 0.0f, 0.0f,
					0.0f, 0.0f, 1.0f, 0.0f,
					0.0f, 0.0f, 0.0f, 1.0f
					);
				break;
			default:
				rotation = DXGI_MODE_ROTATION_IDENTITY;
				m_OrientationTransform3D = XMFLOAT4X4( // 0-degree Z-rotation
					1.0f, 0.0f, 0.0f, 0.0f,
					0.0f, 1.0f, 0.0f, 0.0f,
					0.0f, 0.0f, 1.0f, 0.0f,
					0.0f, 0.0f, 0.0f, 1.0f
					);
				break;
		}

	//	hResult = m_ComPtrSwapChain->SetRotation(rotation);
   // Can't use SetRotation without DXGI_SWAP_CHAIN_DESC1.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL
   // Can't use MultiSampling with DXGI_SWAP_CHAIN_DESC1.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL
   // MultiSampling requires a Bit-Block Transfer(bitblt) model
		if(SUCCEEDED(hResult))
		{// Create a render target view of the swap chain back buffer.
         Microsoft::WRL::ComPtr<ID3D11Texture2D> pBackBuffer;
			hResult = m_ComPtrSwapChain->GetBuffer(0U, __uuidof(ID3D11Texture2D), &pBackBuffer);
			if(SUCCEEDED(hResult))
			{
				hResult = m_ComPtrD3DDevice->CreateRenderTargetView(pBackBuffer.Get(), __nullptr, &m_ComPtrRenderTargetView);
				if(SUCCEEDED(hResult))
				{// Create a depth stencil view.
					CD3D11_TEXTURE2D_DESC 
                  depthStencilDesc(DXGI_FORMAT_D24_UNORM_S8_UINT, uiWidth, uiHeight, 1, 1, D3D11_BIND_DEPTH_STENCIL, D3D11_USAGE_DEFAULT, 0, m_DXGI_SampleDesc.Count, m_DXGI_SampleDesc.Quality, 0);
					ID3D11Texture2D* pDepthStencil;
					hResult = m_ComPtrD3DDevice->CreateTexture2D(&depthStencilDesc, __nullptr, &pDepthStencil);
					if(SUCCEEDED(hResult))
					{
                  CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2DMS);
						hResult = m_ComPtrD3DDevice->CreateDepthStencilView(pDepthStencil, &depthStencilViewDesc, &m_ComPtrDepthStencilView);
						pDepthStencil->Release();
					}
               if(SUCCEEDED(hResult))
               {
                  Microsoft::WRL::ComPtr<IDXGISurface2> pIDXGISurface2;
                  hResult = pBackBuffer.As(&pIDXGISurface2);
                  if(SUCCEEDED(hResult))
                  {
                     D2D1_RENDER_TARGET_PROPERTIES rtProps = D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_DEFAULT, D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED), 96, 96);
                     hResult = m_ComPtrD2DFactory1->CreateDxgiSurfaceRenderTarget(pIDXGISurface2.Get(), rtProps, m_ComPtrD2DRenderTarget.ReleaseAndGetAddressOf());
                     if(SUCCEEDED(hResult))
                     {
                        if(__nullptr == m_ComPtrWhiteBrush)
                        {
                           hResult = m_ComPtrD2DRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White, 1.f), m_ComPtrWhiteBrush.ReleaseAndGetAddressOf());
                        }
                     }
                  }
               }

               if(FAILED(hResult))
               {
                  return hResult;
               }
            }
			}
		}

		// Set the rendering viewport to target the entire window.
		CD3D11_VIEWPORT viewport(0.0f, 0.0f, float(uiWidth), float(uiHeight));

		m_ComPtrImmediateContext->RSSetViewports(1, &viewport);

		float aspectRatio = float(uiWidth) / float(uiHeight);
		float fovAngleY = 70.0f * XM_PI / 180.0f;

		// Note that the m_orientationTransform3D matrix is post-multiplied here
		// in order to correctly orient the scene to match the display orientation.
		// This post-multiplication step is required for any draw calls that are
		// made to the swap chain render target. For draw calls to other targets,
		// this transform should not be applied.
		XMStoreFloat4x4(
			&m_constantBufferData.projection,
			XMMatrixTranspose(
				XMMatrixMultiply(
					XMMatrixPerspectiveFovLH(
						fovAngleY,
						aspectRatio,
						0.01f,
						100.0f
						),
					XMLoadFloat4x4(&m_OrientationTransform3D)
					)
				)
			);
	}

	return hResult;
}