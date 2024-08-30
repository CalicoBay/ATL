//ATLEdit.h: Wraps a EDIT control to hanldle ANSI and Unicode encoding.
#pragma once

const COMDLG_FILTERSPEC c_ATLEditFileTypes[] =
{
	{L"Text Document (*.txt)", L"*.txt"},
	{L"Points Document (*.pts)", L"*.pts"},
	{L"All Documents (*.*)", L"*.*"}
};

//Indices of file types 1 based
const UINT c_IndexText = 1U;
const UINT c_IndexPoints = 2U;

typedef union tagByteOrderMark
{
	tagByteOrderMark(DWORD bom = 0UL){dwBom = bom;}
	BYTE BOM[4];
	WORD wBom[2];
	DWORD dwBom;
}ByteOrderMark;

class CBomFile : public CAtlFile
{
public:
	CBomFile() : m_ullPosition(0ULL), m_ullSize(0ULL), m_BOM(0UL){}
	HRESULT ReadEncoding();
	HRESULT Write(LPVOID pBuffer, DWORD cbSize, PDWORD pBytesWritten = __nullptr);
	int GetEncoding();
	HRESULT GetSize(ULONG64& nLen);
	const PBYTE GetBOM(){return m_BOM.BOM;}
protected:
	ULONG64 m_ullPosition;
	ULONG64 m_ullSize;
	ByteOrderMark m_BOM;
};

class CATLEdit : public CWindowImpl<CATLEdit, CWindow, CWinTraits<WS_CHILD|WS_VISIBLE|WS_VSCROLL|ES_MULTILINE|ES_AUTOVSCROLL|ES_NOHIDESEL>>
{
public:
	CATLEdit() : m_cbShadowBuffer(0), m_UTF(0){}
   ~CATLEdit(){ m_ShadowBuffer.Free(); }
	DECLARE_WND_SUPERCLASS(_T("CATLEdit"), _T("EDIT"))
	BEGIN_MSG_MAP(CATLEdit)
	END_MSG_MAP()
	HRESULT OpenFile(LPCWSTR wszFilePath, DWORD dwAccess, DWORD dwShare, DWORD dwCreate);
	HRESULT SaveFile(LPCWSTR wszFilePath = __nullptr);
	HRESULT Find(LPFINDREPLACE pFindReplace);
	void LimitText(int nChars = 0)
	{
		ATLENSURE(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, EM_LIMITTEXT, nChars, 0L);
	}
protected:
	CBomFile m_File;
	CHeapPtr<char> m_ShadowBuffer;
	CStringW m_sActivePath;
	ULONG32 m_cbShadowBuffer;
	int m_UTF;
	HRESULT GetBufferByteCount(UINT& cb);
	bool IsDirty(){return (0 == SendMessage(EM_GETMODIFY) ? false : true);}
	LPCWSTR LockBuffer();
	VOID UnlockBuffer();
	HRESULT ReadIn();
	HRESULT WriteOut(CAtlFile& fileToSave);
};

