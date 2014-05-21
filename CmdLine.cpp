// *****************************************************************************
// File: CmdLine.cpp
// *****************************************************************************
// Nitrogen Project Version 1
// Copyright© Faronics Corporation 2004
// Written by Cornelis van Rikxoort
// *****************************************************************************

#include "stdafx.h"
#include "CmdLine.h"

#if defined(_DEBUG)
   #define new DEBUG_NEW
   #undef THIS_FILE
   static char THIS_FILE[] = __FILE__;
#endif

//////////////////
// Parse a command line parameter/token. Just add it to the table.
// 
void CCommandLineInfoEx::ParseParam(const TCHAR* pszParam, BOOL bFlag, BOOL bLast)
{
	if(bFlag)
	{
		// this is a "flag" (begins with / or -)
		CString s(pszParam);
		s.MakeLower();
		m_options[s] = _T("TRUE");		// default value is "TRUE"
		m_sLastOption = pszParam;			// save in case other value specified
	}
	else if(!m_sLastOption.IsEmpty())
	{
		// last token was option: set value
		CString s(m_sLastOption);
		s.MakeLower();
		m_options[s] = pszParam;
		m_sLastOption.Empty(); // clear
	}

	// Call base class so MFC can see this param/token.
	CCommandLineInfo::ParseParam(pszParam, bFlag, bLast);
}

BOOL CCommandLineInfoEx::GetOption(LPCTSTR option, CString& val)
{
	CString s(option);
	s.MakeLower();
	return m_options.Lookup(s, val);
}
