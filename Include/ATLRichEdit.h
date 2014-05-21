//ATLRichEdit.h: Wraps a EDIT control to hanldle ANSI and Unicode encoding.
#pragma once
#include "BomFile.h"
const COMDLG_FILTERSPEC c_RichEditFileTypes[] =
{
	{L"Text Document (*.txt)", L"*.txt"},
	{L"Rich Text Document (*.rtf)", L"*.rtf"},
	{L"All Documents (*.*)", L"*.*"}
};

//First dw of a RTF, ansi anyway??
const DWORD c_dwRTF = 0x74725C7B;//{\rt enough to assume RTF
const DWORD c_dwURTF = 0x72755C7B;//{\ur possibly Unicode RTF
//Indices of file types 1 based
const UINT c_IndexText = 1U;
const UINT c_IndexRtf = 2U;

class CATLRichEdit : public CWindowImpl<CATLRichEdit, CWindow, CWinTraits<WS_CHILD|WS_VISIBLE|WS_VSCROLL|ES_MULTILINE|ES_AUTOVSCROLL|ES_NOHIDESEL|ES_SAVESEL|ES_SELECTIONBAR>>
{
public:
	CATLRichEdit() :
		m_UTF(0),
		m_StreamFormat(0),
		m_NewStreamFormat(SF_TEXT),
		m_bIsStoring(false){}
	DECLARE_WND_SUPERCLASS(_T("CATLRichEdit"), MSFTEDIT_CLASS)
	BEGIN_MSG_MAP(CATLRichEdit)
	END_MSG_MAP()
	HRESULT OpenFile(LPCWSTR wszFilePath, DWORD dwAccess, DWORD dwShare, DWORD dwCreate);
	HRESULT SaveFile(LPCWSTR wszFilePath = __nullptr);
	HRESULT Find(LPFINDREPLACE pFindReplace, bool bReplace = false);
	LPCWSTR GetExtensionHint(UINT& index)
	{
		if(m_NewStreamFormat & SF_TEXT)
		{
			index = c_IndexText;
			return L"txt";
		}
		else
		{
			index = c_IndexRtf;
			return L"rtf";
		}
	}
	static DWORD CALLBACK EditStreamCallBack(DWORD_PTR dwpCookie, LPBYTE pbBuff, LONG cb, LONG *pcb);
protected:
	CBomFile m_File;
	CStringW m_sActivePath;
	int m_UTF;
	int m_StreamFormat;
	int m_NewStreamFormat;
	bool m_bIsStoring;
	bool IsDirty(){return (0 == SendMessage(EM_GETMODIFY) ? false : true);}
	HRESULT ReadIn(bool bSelection = false);
	HRESULT WriteOut(CAtlFile& fileToSave, bool bSelection = false);
	int AskType(bool bSaveAs = false);
	VOID DoBOM(CAtlFile& fileToMark);
};

