//ATLRichEdit.cpp: Wraps a MSFTEDIT_CLASS control to hanldle ANSI and Unicode encoding.
#include "stdafx.h"
#include "ATLRichEdit.h"

class _atlRichEditCookie
{
public:
	CATLRichEdit* m_pATLRichEdit;
	CAtlFile& m_File;
	HRESULT m_hResult;
	_atlRichEditCookie(CATLRichEdit* pRich, CAtlFile& file) : m_pATLRichEdit(pRich), m_File(file), m_hResult(S_OK){}
};

LRESULT CATLRichEdit::OnKeyDown(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	UINT keycode = static_cast<UINT>(wParam);// , LOWORD(lParam), HIWORD(lParam)
	//WPARAM numZoom = 0;
	//LPARAM denZoom = 0;
	//DWORD dwLastError = NO_ERROR;
	LRESULT bReturn = FALSE;
	bHandled = FALSE;
	switch(keycode)
	{
	case VK_ADD:
	case VK_OEM_PLUS:
		if((::GetKeyState(VK_CONTROL) >> 1))
		{
			//MessageBox(_T("Control and Plus pressed"));
			Edit_GetZoom(m_hWnd, &m_numZoom, &m_denZoom);
			if(0 == m_numZoom)
			{
				m_numZoom = 2;
			}
			else
			{
				m_numZoom *= 2;
			}
			if(0 == m_denZoom)
			{
				m_denZoom = 1;
			}
			Edit_SetZoom(m_hWnd, m_numZoom, m_denZoom);
			bHandled = TRUE;
			break;
		}
	case VK_OEM_MINUS:
	case VK_SUBTRACT:
		if((::GetKeyState(VK_CONTROL) >> 1))
		{
			Edit_GetZoom(m_hWnd, &m_numZoom, &m_denZoom);
			//MessageBox(_T("Control and Minus pressed"));
			if(0 == m_numZoom)
			{
				m_numZoom = 1;
			}
			if(0 == m_denZoom)
			{
				m_denZoom = 2;
			}
			else
			{
				m_denZoom *= 2;
			}
			Edit_SetZoom(m_hWnd, m_numZoom, m_denZoom);
			bHandled = TRUE;
			break;
		}
	case VK_NUMPAD0:
	case 0x30:
		if((::GetKeyState(VK_CONTROL) >> 1))
		{
			//MessageBox(_T("Control and Zero pressed"));
			m_numZoom = 0;
			m_denZoom = 0;
			Edit_SetZoom(m_hWnd, m_numZoom, m_denZoom);
			bHandled = TRUE;
			break;
		}
	}
	return 0;
}

int CATLRichEdit::AskType(bool bSaveAs/* = false*/)
{
	int iPush;
	int iRadio;
	int iDefaultRadio = 1;
	int iDefaultButton = 1;
	int iNewStreamFormat = SF_TEXT;
	//BOOL bVerified;
	TASKDIALOGCONFIG tdConfig = {};
	TASKDIALOG_BUTTON pushButtons[3] = {{1, L"Text"}, {2, L"Rich Text"}, {3, L"Cancel"}};
	TASKDIALOG_BUTTON radioButtons[4] = {{1, L"ANSI"}, {2, L"UTF-8"}, {3, L"Unicode"}, {4, L"Unicode Big-Endian"}}; 
    
	tdConfig.pszWindowTitle = L"New Document File Type";
	tdConfig.pszMainInstruction = L"Chose document type\r\nDefault is ANSI text";

	if(bSaveAs)
	{
		if(SF_RTF & m_StreamFormat)
		{
			iDefaultButton = 2;
		}

		if(CP_UTF8 == HIWORD(m_StreamFormat))
		{
			iDefaultRadio = 2;
		}
		else if(1200 == HIWORD(m_StreamFormat))//SF_UNICODE & m_StreamFormat)
		{
			iDefaultRadio = 3;
		}
		else if(1201 == HIWORD(m_StreamFormat))
		{
			iDefaultRadio = 4;
		}
		
		tdConfig.pszWindowTitle = L"Save As Document File Type";
		tdConfig.pszMainInstruction = L"Chose document type.\r\nCurrent settings are indicated.";
	}

	tdConfig.cbSize = sizeof(TASKDIALOGCONFIG);
	tdConfig.hwndParent = m_hWnd;
	tdConfig.hInstance = GetModuleHINSTANCE();
	
	//tdConfig.pszVerificationText = L"Are you sure?";
	tdConfig.cButtons = 3;
	tdConfig.pButtons = pushButtons;
	tdConfig.nDefaultButton = iDefaultButton;
	tdConfig.cRadioButtons = 4;
	tdConfig.pRadioButtons = radioButtons;
	tdConfig.nDefaultRadioButton = iDefaultRadio;

	tdConfig.dwFlags = TDF_VERIFICATION_FLAG_CHECKED|TDF_SIZE_TO_CONTENT;
	if(SUCCEEDED(::TaskDialogIndirect(&tdConfig, &iPush, &iRadio, __nullptr/*&bVerified*/)))
	{
		if(3 > iPush)
		{//if not cancelled
			if(2 == iPush)
			{
				iNewStreamFormat = SF_RTF;
			}
			else
			{
				iNewStreamFormat = SF_TEXT;
			}

			if(2 == iRadio)
			{
				iNewStreamFormat |= (CP_UTF8 << 16)|SF_USECODEPAGE;
			}
			else if(3 == iRadio)
			{
				iNewStreamFormat |= SF_UNICODE;
				iNewStreamFormat |= (1200 << 16)|SF_USECODEPAGE;
			}
			else if(4 == iRadio)
			{
				iNewStreamFormat |= (1201 << 16)|SF_USECODEPAGE;
			}
		}
	}
	return iNewStreamFormat;
}

HRESULT CATLRichEdit::OpenFile(LPCWSTR wszFilePath, DWORD dwAccess, DWORD dwShare, DWORD dwCreate)
{
	if(IsDirty())
	{
		int iResult;
		if(SUCCEEDED(::TaskDialog(__nullptr, __nullptr, __nullptr, m_sActivePath, L"Do you wish to save your changes?", TDCBF_YES_BUTTON|TDCBF_NO_BUTTON|TDCBF_CANCEL_BUTTON, TD_WARNING_ICON, &iResult)))
		{
			if(IDYES == iResult)
			{
				if(FAILED(SaveFile()))
				{
					return S_FALSE;
				}
			}
			else if(IDCANCEL == iResult)
			{
				return S_FALSE;
			}
		}
	}

	if(__nullptr != m_File.m_h)
	{
		m_File.Close();
	}

	SetWindowText(L"");

	HRESULT hr = m_File.Create(wszFilePath, dwAccess, dwShare, dwCreate);
	if(FAILED(hr))
	{//New instead of open existing
		//::TaskDialog(m_hWnd, __nullptr, __nullptr, L"We need to know what type!", __nullptr, TDCBF_OK_BUTTON, TD_INFORMATION_ICON, __nullptr);
		m_NewStreamFormat = AskType();
	}
	else
	{
		hr = ReadIn();
		if(SUCCEEDED(hr))
		{
			m_sActivePath = wszFilePath;
		}
	}

	return hr;
}

HRESULT CATLRichEdit::ReadIn(bool bSelection/* = false*/)
{
	HRESULT hr = S_OK;
	m_StreamFormat = 0;
	m_UTF = m_File.GetEncoding();
	if(-1 == m_UTF)
	{
		hr = E_HANDLE;
	}
	else
	{
		DWORD dwFirst;
		m_File.GetBOM(dwFirst);
		if((c_dwRTF == dwFirst) || (c_dwURTF == dwFirst))
		{
			m_StreamFormat = SF_RTF;
			if(c_dwURTF == dwFirst)
			{
				m_StreamFormat |= (CP_UTF8 << 16)|SF_USECODEPAGE;
			}
		}
		else
		{
			m_StreamFormat = SF_TEXT;
		}

		if(BOM::cpUTF16LE == m_UTF)
		{
			m_StreamFormat |= SF_UNICODE;
			m_StreamFormat |= (1200 << 16)|SF_USECODEPAGE;
		}
		else if(BOM::cpUTF16BE == m_UTF)
		{
			m_StreamFormat |= (1201 << 16)|SF_USECODEPAGE;
		}

		m_bIsStoring = false;
		EDITSTREAM es = {0, 0, &EditStreamCallBack};
		_atlRichEditCookie cookie(this, m_File);
		es.dwCookie = DWORD_PTR(&cookie);
		if(bSelection)
		{
			m_StreamFormat |= SFF_SELECTION;
		}

		LRESULT lReturn = SendMessage(EM_STREAMIN, WPARAM(m_StreamFormat), LPARAM(&es));
		hr = cookie.m_hResult;
	}
	return hr;
}

HRESULT CATLRichEdit::SaveFile(LPCWSTR wszFilePath/* = __nullptr*/)
{
	HRESULT hr = S_FALSE;
	CAtlFile fileToSave;
	CStringW sFilePath(wszFilePath);
	bool bNameChange = false;
	if(sFilePath.IsEmpty())
	{
		sFilePath = m_sActivePath;
	}
	else
	{
		bNameChange = true;
		int iNewStreamFormat = AskType(true);
		m_StreamFormat = 0;
		m_NewStreamFormat = iNewStreamFormat;
	}

	hr = fileToSave.Create(sFilePath, GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, CREATE_ALWAYS);
	if(SUCCEEDED(hr))
	{
		hr = WriteOut(fileToSave);
		if(SUCCEEDED(hr))
		{
			SendMessage(EM_SETMODIFY);//Not Dirty
			if(bNameChange)
			{
				m_sActivePath = sFilePath;
			}

			if(__nullptr != m_File.m_h)
			{
				m_File.Close();
				m_StreamFormat = 0;
				m_UTF = 0;
			}
			fileToSave.Close();
			if(SUCCEEDED(m_File.Create(m_sActivePath, GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE, OPEN_EXISTING)))
			{
				m_UTF = m_File.GetEncoding();
				if(-1 == m_UTF)
				{
					hr = E_HANDLE;
				}
				else
				{
					DWORD dwFirst;
					m_File.GetBOM(dwFirst);
					if((c_dwRTF == dwFirst) || (c_dwURTF == dwFirst))
					{
						m_StreamFormat = SF_RTF;
						if(c_dwURTF == dwFirst)
						{
							m_StreamFormat |= (CP_UTF8 << 16)|SF_USECODEPAGE;
						}
					}
					else
					{
						m_StreamFormat = SF_TEXT;
					}

					if(BOM::cpUTF16LE == m_UTF)
					{
						m_StreamFormat |= SF_UNICODE;
						m_StreamFormat |= (1200 << 16)|SF_USECODEPAGE;
					}
					else if(BOM::cpUTF16BE == m_UTF)
					{
						m_StreamFormat |= (1201 << 16)|SF_USECODEPAGE;
					}
				}
			}
		}
	}
	return hr;
}

HRESULT CATLRichEdit::WriteOut(CAtlFile& fileToSave, bool bSelection/* = false*/)
{
	if(0 == m_StreamFormat)
	{//We're saving a new file, call DoBOM
		DoBOM(fileToSave);
	}

	m_bIsStoring = true;
	EDITSTREAM es = {0, 0, &EditStreamCallBack};
	_atlRichEditCookie cookie(this, fileToSave);
	es.dwCookie = DWORD_PTR(&cookie);
	if(bSelection)
	{
		m_StreamFormat |= SFF_SELECTION;
	}

	LRESULT lReturn = SendMessage(EM_STREAMOUT, WPARAM(m_StreamFormat), LPARAM(&es));
	HRESULT hr = cookie.m_hResult;
	m_bIsStoring = false;
	return hr;
}

VOID CATLRichEdit::DoBOM(CAtlFile& fileToMark)
{
	m_StreamFormat = m_NewStreamFormat;
	if(SF_TEXT & m_StreamFormat)
	{//No need to mark SF_RTF
		if(CP_UTF8 == HIWORD(m_StreamFormat))
		{
			BYTE utf8BOM[3] = {0xEF, 0xBB, 0xBF};
			fileToMark.Write(utf8BOM, 3);
		}
		else if(1200 == HIWORD(m_StreamFormat))//SF_UNICODE & m_StreamFormat)
		{
			BYTE utf16BOM[2] = {0xFF, 0xFE};
			fileToMark.Write(utf16BOM, 2);
		}
		else if(1201 == HIWORD(m_StreamFormat))
		{
			BYTE utf16BeBOM[2] = {0xFE, 0xFF};
			fileToMark.Write(utf16BeBOM, 2);
		}
	}
}

HRESULT CATLRichEdit::Find(LPFINDREPLACE pFindReplace, bool bReplace/* = false*/)
{
	HRESULT hr = S_FALSE;
	if(__nullptr != pFindReplace)
	{
		if(_T('\0') != pFindReplace->lpstrFindWhat[0])
		{
			LRESULT lr;
			CHARRANGE cr = {};
			SendMessage(EM_EXGETSEL, WPARAM(0), LPARAM(&cr));

			FINDTEXTEX ft = {};
			if(FR_DOWN & pFindReplace->Flags)
			{
				ft.chrg.cpMin = cr.cpMax;
				ft.chrg.cpMax = -1;
			}
			else
			{
				ft.chrg.cpMin = cr.cpMin;
				ft.chrg.cpMax = 0;
			}

			ft.lpstrText = pFindReplace->lpstrFindWhat;
			lr = SendMessage(EM_FINDTEXTEX, WPARAM(pFindReplace->Flags), LPARAM(&ft));
			if(-1L < lr)
			{
				cr.cpMax = ft.chrgText.cpMax;
				cr.cpMin = ft.chrgText.cpMin;
			}

			SendMessage(EM_EXSETSEL, WPARAM(0), LPARAM(&cr));
			if(-1L < lr)
			{
				hr = S_OK;
				if(bReplace)
				{
					SETTEXTEX st = {ST_SELECTION|ST_KEEPUNDO|ST_UNICODE, UINT(-1)};
					if(SF_UNICODE & m_StreamFormat)
					{
						st.flags |= ST_UNICODE;
					}
					else if(SF_USECODEPAGE & m_StreamFormat)
					{
						st.codepage = HIWORD(m_StreamFormat);
					}
					lr = SendMessage(EM_SETTEXTEX, WPARAM(&st), LPARAM(pFindReplace->lpstrReplaceWith));
					if(1 > lr)
					{
						hr = S_FALSE;
					}
				}
			}
		}
	}
	if(S_FALSE == hr)
	{
		::MessageBeep(MB_ICONWARNING);
	}
	else if(S_OK == hr)
	{
		::MessageBeep(MB_ICONASTERISK);
	}
	else
	{
		::MessageBeep(MB_ICONERROR);
	}
	return hr;
}

DWORD CALLBACK CATLRichEdit::EditStreamCallBack(DWORD_PTR dwpCookie, LPBYTE pbBuff, LONG cb, LONG *pcb)
{
	DWORD dwReturn = 0;
	_atlRichEditCookie* pCookie = (_atlRichEditCookie*)dwpCookie;
	CATLRichEdit* pCATLRichEdit = pCookie->m_pATLRichEdit;
	CAtlFile& file = pCookie->m_File;
	*pcb = cb;
	DWORD dwReadOrWritten, dwBytes = DWORD(cb);
	if(pCATLRichEdit->m_bIsStoring)
	{
		pCookie->m_hResult = file.Write(pbBuff, dwBytes, &dwReadOrWritten);
	}
	else
	{
		pCookie->m_hResult = file.Read(pbBuff, dwBytes, dwReadOrWritten);
	}
	if(SUCCEEDED(pCookie->m_hResult))
	{
		*pcb = LONG(dwReadOrWritten);
	}
	else if(STRSAFE_E_END_OF_FILE == pCookie->m_hResult)
	{
		*pcb = LONG(dwReadOrWritten);
	}
	else
	{
		dwReturn = 1;
	}
	
	return dwReturn;
}