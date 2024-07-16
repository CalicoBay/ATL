#pragma once
#include "D3DTab.h"

struct ModelViewProjectionConstantBuffer
{
	DirectX::XMFLOAT4X4 model;
	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT4X4 projection;
};

struct VertexPositionColor
{
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 color;
};

class CD3DCubeTab : public CD3DTab
{
public:
	CD3DCubeTab(void);
	~CD3DCubeTab(void);
   HRESULT Initialize(HWND hwndParent, LPCTSTR tszText = __nullptr, bool bJustDirectX = false);

protected:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_ComPtrVertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_ComPtrPixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_ComPtrInputLayout;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_ComPtrVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_ComPtrIndexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_ComPtrConstantBuffer;
	UINT m_indexCount;
	ModelViewProjectionConstantBuffer m_constantBufferData;
	XMFLOAT4X4 m_OrientationTransform3D;
	UINT m_Orientation;
	//HRESULT ReadData(LPCTSTR csFile, PCHAR& pBuffer, size_t& uiCount);
   //HRESULT LoadShaderResource(PCTSTR ptszResource, PCHAR& pBuffer, size_t& uiCount);
	HRESULT m_hrInitializeCube;
	VOID Render();
	HRESULT RecreateSizedResources(UINT uiWidth, UINT uiHeight);
   VOID OnFinalMessage(HWND /*hwnd*/){delete this;}
};

