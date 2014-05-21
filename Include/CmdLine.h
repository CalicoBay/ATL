// *****************************************************************************
// File: CmdLine.h
// *****************************************************************************
// Nitrogen Project Version 1
// Copyright© Faronics Corporation 2004
// Written by Cornelis van Rikxoort
// *****************************************************************************

#pragma once

// Improved CCommandLineInfo parses arbitrary switches.
// Use instead of CCommandLineInfo:
//
//    CCommandLineInfoEx cmdinfo;
//    ParseCommandLine(cmdinfo); // (from app object)
//
// After parsing, you can call GetOption to get the value of any switch. Eg:
//    To get -nologo or /nologo
//    if (cmdinfo.GetOption("nologo")) {
//       // handle it
//    }
//
// to get the value of a string option, type
//
//    CString filename;
//    if (cmdinfo.GetOption("f", filename)) {
//       // now filename is string following -f or /f option
//    }
//
class CCommandLineInfoEx : public CCommandLineInfo
{
public:
   BOOL GetOption(LPCTSTR option, CString& val);
   BOOL GetOption(LPCTSTR option)
   {
      return GetOption(option, CString());
   }
protected:
   CMapStringToString m_options; // hash of options
   CString  m_sLastOption;       // last option encountered
   virtual void ParseParam(const TCHAR* pszParam, BOOL bFlag, BOOL bLast);
};
