#pragma once
using namespace DirectX;

class CD3DTab : public CWindowImpl<CD3DTab, CWindow, CWinTraits<WS_CHILD|WS_VISIBLE>>
{
public:
	CD3DTab();
	~CD3DTab();
	DECLARE_WND_CLASS_EX(__nullptr, CS_DBLCLKS, -1);
	HRESULT Initialize(HWND hwndParent, LPCTSTR tszText = __nullptr, bool bJustDirectX = false);
	VOID SetSuper(){m_bSuper = true;}
   static float t;
protected:
   CString m_csCurrentDir;
   CStringW m_csInitText;
   CStringW m_csText0;
	bool m_bAnimated;
   BOOL m_bCurrentFrontCCW;
   BOOL m_bMultiSampling;
   D3D11_FILL_MODE m_CurrentFillMode;
   D3D11_CULL_MODE m_CurrentCullMode;
	D3D_DRIVER_TYPE         m_D3D_Driver_Type;
	D3D_FEATURE_LEVEL       m_D3D_Feature_Level;
   DXGI_SAMPLE_DESC m_DXGI_SampleDesc;
	Microsoft::WRL::ComPtr<ID3D11Device1>           m_ComPtrD3DDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext1>    m_ComPtrImmediateContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain1>         m_ComPtrSwapChain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>	m_ComPtrRenderTargetView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>	m_ComPtrDepthStencilView;
   Microsoft::WRL::ComPtr<ID3D11RasterizerState1> m_ComPtrRastCurrent;
   Microsoft::WRL::ComPtr<ID2D1Factory1> m_ComPtrD2DFactory1;
   Microsoft::WRL::ComPtr<IDWriteFactory1> m_ComPtrIDWriteFactory1;
   Microsoft::WRL::ComPtr<ID2D1RenderTarget> m_ComPtrD2DRenderTarget;
   Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_ComPtrWhiteBrush;
   Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_ComPtrTextBrush;
   Microsoft::WRL::ComPtr<IDWriteTextFormat> m_ComPtrDWriteTextFormat0;
   Microsoft::WRL::ComPtr<ID3D11Texture2D> m_ComPtrID3D11Texture2DOffscreen;
   Microsoft::WRL::ComPtr<ID2D1RenderTarget> m_ComPtrDXGISurfaceRenderTarget;
   float				m_bkgrndColor[4];
	DXGI_PRESENT_PARAMETERS m_PresentParams;
	HRESULT m_hrInitializeDirect3D;
   HRESULT m_hrInitializeIndependent;
	HRESULT InitializeDirect3D();
   HRESULT InitializeIndependent();
   BEGIN_MSG_MAP(CD3DTab)
      MESSAGE_HANDLER(WM_CONTEXTMENU, OnContextMenu)
      MESSAGE_HANDLER(WM_CREATE, OnCreate)
      MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_DISPLAYCHANGE, OnDisplayChange)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
      MESSAGE_HANDLER(WM_TIMER, OnTimer)
	END_MSG_MAP()
	VOID Clear();
	virtual VOID Render();
   LRESULT OnContextMenu(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled); 
   LRESULT OnDisplayChange(UINT /*nMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	virtual HRESULT RecreateSizedResources(UINT uiWidth, UINT uiHeight);
   VOID OnFinalMessage(HWND /*hwnd*/){if(!m_bSuper)delete this; }
	LRESULT OnCreate(UINT /*nMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
   LRESULT OnDestroy(UINT /*nMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/){ KillTimer(0); return 0; }
   LRESULT OnPaint(UINT /*nMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnSize(UINT /*nMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
   LRESULT OnTimer(UINT /*nMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/){ InvalidateRect(__nullptr, FALSE); return 0; }
private:
	bool m_bSuper;
};

