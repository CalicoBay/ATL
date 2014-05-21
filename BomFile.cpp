#include "stdafx.h"
#include "BomFile.h"

HRESULT CBomFile::GetSize(ULONG64& nLen)
{
	HRESULT hr = __super::GetSize(nLen);
	m_ullSize = 0ULL;
	if(SUCCEEDED(hr))
	{
		m_ullSize = nLen;
	}
	return hr;
}

HRESULT CBomFile::ReadEncoding()
{
	HRESULT hr = GetPosition(m_ullPosition);
	if(SUCCEEDED(hr))
	{
		if(0ULL == m_ullPosition && 0ULL < m_ullSize)
		{
			DWORD dwToRead = sizeof(m_BOM);
			DWORD dwBytesRead = 0;
			do
			{
				hr = __super::Read(&m_BOM, dwToRead--, dwBytesRead);
				if(0 == dwToRead)
				{
					break;
				}
			}while(STRSAFE_E_END_OF_FILE == hr);
		}
	}
	Seek(0ULL, FILE_BEGIN);
	return hr;
}

int CBomFile::GetEncoding()
{//These comparisons assume we are on a little endian platform, the little end is first in memory.
	size_t cbLength;
	int iUTF = -1;
	if(SUCCEEDED(GetSize(cbLength)))
	{
		if(SUCCEEDED(ReadEncoding()))
		{
			iUTF = 0;
			if(0xFEFF == m_BOM.wBom[0])
			{//UTF16
				iUTF = BOM::cpUTF16LE;
			}
			else if(0xFFFE == m_BOM.wBom[0])
			{//UTF16 Big-Endian
				iUTF = BOM::cpUTF16BE;
			}
			else if(0x0000 == m_BOM.wBom[0] && 0xFEFF == m_BOM.wBom[1])
			{//UTF32
				iUTF = BOM::cpUTF32LE;
			}
			else if(0xFFFE == m_BOM.wBom[0] && 0x0000 == m_BOM.wBom[1])
			{//UTF32 Big-Endian
				iUTF = BOM::cpUTF32BE;
			}
			else if(0xBBEF == m_BOM.wBom[0])//0xEFBB
			{
				USHORT usCheck = (m_BOM.wBom[1] & 0x00FF);//0xFF00
				if(0x00BF == usCheck)//0xBF00
				{//UTF8
					iUTF = BOM::cpUTF8;
				}
			}
			else if((0x1F < m_BOM.BOM[0]) && (0x7F > m_BOM.BOM[0]))
			{
				if(0 == m_BOM.BOM[1])
				{//Unencoded UTF16
					iUTF = BOM::cpUTF16LE;
				}
			}
			else if((0x1F < m_BOM.BOM[1]) && (0x7F > m_BOM.BOM[1]))
			{
				if(0 == m_BOM.BOM[0])
				{//Unencoded UTF16 Big-Endian
					iUTF = BOM::cpUTF16BE;
				}
			}
		}
	}

	return iUTF;
}

