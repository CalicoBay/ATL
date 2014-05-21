#include "stdafx.h"

BOOL DisplayError(LPCTSTR sErrorName, ULONG ulError)
{
   LPVOID lpMessageBuffer = NULL;
   CString sMessage;

   if(::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                              NULL, ulError, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                                                   (LPTSTR) &lpMessageBuffer, 0, NULL))
   {
      sMessage.Format(TEXT("%s FAILURE(%u): %s"), sErrorName, ulError, (TCHAR*)lpMessageBuffer);
      ::AfxMessageBox(sMessage);
   }
   else
   {
      sMessage.Format(TEXT("%s FAILURE: (0x%08x)"),sErrorName, ulError);
      ::AfxMessageBox(sMessage);
   }

   if(NULL != lpMessageBuffer)
   {
      ::LocalFree(lpMessageBuffer); // Free system buffer 
   }

   return FALSE;
}

