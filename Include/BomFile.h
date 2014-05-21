#pragma once

namespace BOM
{
	static const int cpUTF7 = 7;
	static const int cpUTF8 = 8;
	static const int cpUTF16LE = 16;
	static const int cpUTF16BE = 17;
	static const int cpUTF32LE = 32;
	static const int cpUTF32BE = 33;
}

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
	int GetEncoding();
	HRESULT GetSize(ULONG64& nLen);
	const PBYTE GetBOM(){return m_BOM.BOM;}
	void GetBOM(DWORD& dwBom){dwBom = m_BOM.dwBom;}
protected:
	ULONG64 m_ullPosition;
	ULONG64 m_ullSize;
	ByteOrderMark m_BOM;
	HRESULT ReadEncoding();
};

