//ATLEdit.cpp: Wraps a EDIT control to hanldle ANSI and Unicode encoding.
#include "stdafx.h"
#include "ATLEdit.h"

HRESULT CATLEdit::OpenFile(LPCWSTR wszFilePath, DWORD dwAccess, DWORD dwShare, DWORD dwCreate)
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
	if(SUCCEEDED(hr))
	{
		hr = ReadIn();
	}

	if(SUCCEEDED(hr))
	{
		m_sActivePath = wszFilePath;
	}

	return hr;
}

HRESULT CATLEdit::ReadIn()
{//If its ANSI we need a shadow buffer, otherwise it doesn't display properly
	HRESULT hr = S_OK;
	
	size_t cbLength;
	m_File.GetSize(cbLength);
	LRESULT limit = SendMessage(EM_GETLIMITTEXT);
	if(size_t(limit) < cbLength)
	{
		hr = TYPE_E_SIZETOOBIG;
	}
	else
	{
		if(__nullptr != m_ShadowBuffer)
		{
			m_ShadowBuffer.Free();
		}
		m_cbShadowBuffer = 0;

		LPVOID hBytes = ::LocalAlloc(LMEM_MOVEABLE, cbLength + 2U);
		if(__nullptr == hBytes)
		{//Add a couple of bytes for the terminating \0
			hr = E_OUTOFMEMORY;
		}
		else
		{
			LPBYTE pBytes = LPBYTE(::LocalLock(hBytes));
			if(__nullptr == pBytes)
			{
				::LocalFree(hBytes);
				hr = E_OUTOFMEMORY;
			}
			if(SUCCEEDED(hr))
			{
				pBytes[cbLength] = '\0';
				pBytes[cbLength + 1] = '\0';
				hr = m_File.ReadEncoding();
				if(SUCCEEDED(hr))
				{
					hr = m_File.Read(pBytes, DWORD(cbLength));
				}
				else
				{
					::LocalUnlock(hBytes);
					::LocalFree(hBytes);
					return hr;
				}

				if(SUCCEEDED(hr))
				{
					int iUTF = m_UTF = m_File.GetEncoding(); 
					if(0 == iUTF)
					{
						::SetWindowTextA(m_hWnd, LPCSTR(pBytes));
						::SendMessageA(m_hWnd, EM_SETSEL, cbLength, cbLength);
						//SetFocus();

						::LocalUnlock(hBytes);
						::LocalFree(hBytes);
						return hr;
					}
					else if(8 == iUTF)
					{
						::SetWindowTextA(m_hWnd, LPCSTR(&pBytes[3]));
						::SendMessageA(m_hWnd, EM_SETSEL, cbLength -3, cbLength - 3);
						//SetFocus();

						::LocalUnlock(hBytes);
						::LocalFree(hBytes);
						return hr;
					}
					else if(17 == iUTF)// Big-Endian
					{
						if(SUCCEEDED(hr))
						{
							if(0 == (cbLength % 2))
							{
								for(UINT i = 1; i < cbLength; i += 2)
								{
									byte least = pBytes[i - 1];
									pBytes[i - 1] = pBytes[i];
									pBytes[i] = least;
								}
							}
						}
					}
				}
				::LocalUnlock(hBytes);
				HLOCAL hOldText = HLOCAL(SendMessage(EM_GETHANDLE));
				::LocalFree(hOldText);
				SendMessage(EM_SETHANDLE, WPARAM(hBytes));
				SendMessage(EM_SETSEL, cbLength >> 1, cbLength >> 1);
				//SetFocus();
			}
		}
	}
	return hr;
}

HRESULT CATLEdit::SaveFile(LPCWSTR wszFilePath/* = __nullptr*/)
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
				m_File.Attach(fileToSave.Detach());
			}
		}
	}
	return hr;
}

HRESULT CATLEdit::WriteOut(CAtlFile& fileToSave)
{
	HRESULT hr;
	size_t cbLen;
	LPCWSTR pwszText = LockBuffer();
	if(16 > m_UTF)
	{
		hr = ::StringCbLengthA(LPCSTR(pwszText), STRSAFE_MAX_CCH, &cbLen);
	}
	else
	{
		hr = ::StringCbLengthW(pwszText, STRSAFE_MAX_CCH, &cbLen);
	}

	if(SUCCEEDED(hr))
	{
		switch(m_UTF)
		{
		case 8:
			{
				hr = fileToSave.Write(m_File.GetBOM(), 3);
				if(SUCCEEDED(hr))
				{
					hr = fileToSave.Write(pwszText, DWORD(cbLen));
				}
				break;
			}
		case 17:
			{//Allocate a char buffer so we can swap
				CHeapPtr<char> Buffer;
				if(Buffer.Allocate(cbLen + 2))
				{
					hr = ::StringCbCopyW(LPWSTR(Buffer.m_pData), cbLen + 2, pwszText);
					if(SUCCEEDED(hr))
					{
						for(int i = 1; i < cbLen; i += 2)
						{
							byte least = Buffer.m_pData[i - 1];
							Buffer.m_pData[i - 1] = Buffer.m_pData[i];
							Buffer.m_pData[i] = least;
						}

						hr = fileToSave.Write(Buffer, DWORD(cbLen));
					}
				}
				break;
			}
		default://UTF16 or 0
			{
				hr = fileToSave.Write(pwszText, DWORD(cbLen));
				break;
			}
		}
	}
	UnlockBuffer();
	return hr;
}

LPCWSTR CATLEdit::LockBuffer()
{
	LPCWSTR pwszText = __nullptr;
	if(__nullptr != m_hWnd)
	{
		if(16 > m_UTF)
		{
         bool bModified = IsDirty();
         if(__nullptr == m_ShadowBuffer || bModified)
			{
				_ASSERTE(__nullptr != m_ShadowBuffer || 0 == m_cbShadowBuffer);
				UINT nSize = ::GetWindowTextLengthA(m_hWnd) + 1;
				if(nSize > m_cbShadowBuffer)
				{
					m_ShadowBuffer.ReallocateBytes(nSize);
					m_cbShadowBuffer = nSize;
				}
				if(__nullptr != m_ShadowBuffer)
				{
					::GetWindowTextA(m_hWnd, m_ShadowBuffer, nSize);
				}
			}

			return LPCWSTR(m_ShadowBuffer.m_pData);
		}
		else
		{
			HLOCAL hLocal = HLOCAL(SendMessage(EM_GETHANDLE));
			if(__nullptr != hLocal)
			{
				pwszText = LPCWSTR(::LocalLock(hLocal));
			}
		}
	}
	return pwszText;
}

VOID CATLEdit::UnlockBuffer()
{
	if(16 > m_UTF)
	{
		return;
	}

	if(__nullptr != m_hWnd)
	{
		HLOCAL hLocal = HLOCAL(SendMessage(EM_GETHANDLE));
		if(__nullptr != hLocal)
		{
			::LocalUnlock(hLocal);
		}
	}
}

HRESULT CATLEdit::GetBufferByteCount(UINT& cb)
{
	HRESULT hr;
	size_t cbLen;

	LPCWSTR pwszText = LockBuffer();
	if(16 > m_UTF)
	{
		hr = ::StringCbLengthA(LPCSTR(pwszText), STRSAFE_MAX_CCH, &cbLen);
	}
	else
	{
		hr = ::StringCbLengthW(pwszText, STRSAFE_MAX_CCH, &cbLen);
	}

	if(SUCCEEDED(hr))
	{
		cb = UINT(cbLen);
	}

	UnlockBuffer();
	return hr;
}

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
			if(SUCCEEDED(hr))
			{
				Seek(0ULL, FILE_BEGIN);
			}
		}
	}

	return hr;
}

HRESULT CBomFile::Write(LPVOID pBuffer, DWORD cbSize, PDWORD pBytesWritten/* = __nullptr*/)
{
	HRESULT hr;
	hr = __super::Write(pBuffer, cbSize);
	return hr;
}

int CBomFile::GetEncoding()
{//These comparisons assume we are on a little endian platform, the little end is first in memory.
	int iUTF = 0;
	if(0xFEFF == m_BOM.wBom[0])
	{//UTF16
		iUTF = 16;
	}
	else if(0xFFFE == m_BOM.wBom[0])
	{//UTF16 Big-Endian
		iUTF = 17;
	}
	else if(0x0000 == m_BOM.wBom[0] && 0xFEFF == m_BOM.wBom[1])
	{//UTF32
		iUTF = 32;
	}
	else if(0xFFFE == m_BOM.wBom[0] && 0x0000 == m_BOM.wBom[1])
	{//UTF32 Big-Endian
		iUTF = 33;
	}
	else if(0xBBEF == m_BOM.wBom[0])//0xEFBB
	{
		USHORT usCheck = (m_BOM.wBom[1] & 0x00FF);//0xFF00
		if(0x00BF == usCheck)//0xBF00
		{//UTF8
			iUTF = 8;
		}
	}
	else if((0x1F < m_BOM.BOM[0]) && (0x7F > m_BOM.BOM[0]))
	{
		if((1ULL < m_ullSize) && (0 == m_BOM.BOM[1])) //One character ANSI or UTF-8 unencoded files were passing this test.
		{//Unencoded UTF16
			iUTF = 16;
		}
	}
	else if((0x1F < m_BOM.BOM[1]) && (0x7F > m_BOM.BOM[1]))
	{
		if(0 == m_BOM.BOM[0])
		{//Unencoded UTF16 Big-Endian
			iUTF = 17;
		}
	}

	return iUTF;
}

HRESULT CATLEdit::Find(LPFINDREPLACE pFindReplace)
{
	HRESULT hr = S_FALSE;
	if(__nullptr != pFindReplace)
	{
		if(_T('\0') != pFindReplace->lpstrFindWhat[0])
		{
			bool bMatchCase = (FR_MATCHCASE & pFindReplace->Flags) ? true : false;
			bool bWholeWord = (FR_WHOLEWORD & pFindReplace->Flags) ? true : false;
			bool bDown		= (FR_DOWN & pFindReplace->Flags) ? true : false;

			UINT cbLen;
			hr = GetBufferByteCount(cbLen);
			if(SUCCEEDED(hr))
			{
				DWORD dwStartChar, dwEndChar;
				//if(16 > m_UTF)
				//{
				//	::SendMessageA(m_hWnd, EM_GETSEL, WPARAM(&dwStartChar), LPARAM(&dwEndChar));
				//}
				//else
				//{
					SendMessage(EM_GETSEL, WPARAM(&dwStartChar), LPARAM(&dwEndChar));
				//}
				DWORD dwStart = dwStartChar;
				int iPosition;
				if(0 == dwStart && !bDown)
				{//No search before the start!
					::MessageBeep(MB_ICONWARNING);
					return S_FALSE;
				}

				BYTE cbChar;
				CStringW sToFind(pFindReplace->lpstrFindWhat);
				CStringW sContent;
				LPCWSTR pwszText = LockBuffer();
				if(16 > m_UTF)
				{
					cbChar = sizeof(CHAR);
					sContent.Format(L"%S", pwszText);
				}
				else
				{
					cbChar = sizeof(WCHAR);
					sContent.Format(L"%s", pwszText);
				}

				
				if(bDown)
				{
					iPosition = sContent.Find(sToFind, dwStart);
				}
				else
				{
					iPosition = sContent.Find(sToFind, dwStart);
				}

				hr = 0 > iPosition ? S_FALSE : S_OK;
				UnlockBuffer();
				//if(16 > m_UTF)
				//{
				//	::SendMessageA(m_hWnd, EM_SETSEL, WPARAM(iPosition), LPARAM(iPosition + sToFind.GetLength()));
				//}
				//else
				//{
					SendMessage(EM_SETSEL, WPARAM(iPosition), LPARAM(iPosition + sToFind.GetLength()));
				//}
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
