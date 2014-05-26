// PolyProp.h : Declaration of the CPolyProp


#pragma once
#include "resource.h"       // main symbols


#include "Polygon_i.h"

using namespace ATL;

// CPolyProp

class ATL_NO_VTABLE CPolyProp :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CPolyProp, &CLSID_PolyProp>,
	public IPropertyPageImpl<CPolyProp>,
	public ATL::CDialogImpl<CPolyProp>
{
public:
	CPolyProp()
	{
		m_dwTitleID = IDS_TITLEPOLYPROP;
		m_dwHelpFileID = IDS_HELPFILEPOLYPROP;
		m_dwDocStringID = IDS_DOCSTRINGPOLYPROP;
	}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

	enum {IDD = IDD_POLYPROP};

DECLARE_REGISTRY_RESOURCEID(IDR_POLYPROP)


BEGIN_COM_MAP(CPolyProp)
	COM_INTERFACE_ENTRY(IPropertyPage)
END_COM_MAP()

BEGIN_MSG_MAP(CPolyProp)
	CHAIN_MSG_MAP(IPropertyPageImpl<CPolyProp>)
	COMMAND_HANDLER(IDC_SIDES, EN_CHANGE, OnEnChangeSides)
END_MSG_MAP()

// Handler prototypes:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

	STDMETHOD(Apply)(void)
	{
		USES_CONVERSION;
		ATLTRACE(_T("CPolyProp::Apply\n"));
		for (UINT i = 0; i < m_nObjects; i++)
		{
			CComQIPtr<IPolyCtl, &IID_IPolyCtl> pPoly(m_ppUnk[i]);
			short nSides = (short)GetDlgItemInt(IDC_SIDES);
			if FAILED(pPoly->put_Sides(nSides))
			{
				CComPtr<IErrorInfo> pError;
				CComBSTR strError;
				GetErrorInfo(0, &pError);
				pError->GetDescription(&strError);
				MessageBox(OLE2T(strError), _T("Error"), MB_ICONEXCLAMATION);
				return E_FAIL;
			}
		}
		m_bDirty = FALSE;
		return S_OK;
	}
	LRESULT OnEnChangeSides(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
};


OBJECT_ENTRY_AUTO(__uuidof(PolyProp), CPolyProp)
