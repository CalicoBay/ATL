#include "pch.h"
#include "EditableLayout.h"
#include "DrawingEffects.h"
#include "RenderTarget.h"
#include "InlineImage.h"
#include "DWriteTab.h"

#pragma comment(linker, "/defaultlib:d3d11.lib")
#pragma comment(linker, "/defaultlib:d2d1.lib")
#pragma comment(linker, "/defaultlib:dwrite.lib")
using namespace DirectX;

CDWriteTab::CDWriteTab() :
   m_bAnimated(false),
   m_currentlySelecting(false),
   m_currentlyPanning(false),
   m_previousMouseX(0),
   m_previousMouseY(0),
   m_dpiScaleX(1.0),
   m_dpiScaleY(1.0),
   m_viewScaleX(1.0),
   m_viewScaleY(1.0),
   m_Angle(0),
   m_OriginX(0),
   m_OriginY(0),
   m_hWndParent(__nullptr),
   m_caretAnchor(0),
   m_caretPosition(0),
   m_caretPositionOffset(0)
{
}

VOID CDWriteTab::AlignCaretToNearestCluster(bool isTrailingHit, bool skipZeroWidth)
{
   // Uses hit-testing to align the current caret position to a whole cluster,
   // rather than residing in the middle of a base character + diacritic,
   // surrogate pair, or character + UVS.

   DWRITE_HIT_TEST_METRICS hitTestMetrics;
   float caretX, caretY;

   // Align the caret to the nearest whole cluster.
   m_CComPtrDWriteTextLayout0->HitTestTextPosition(
      m_caretPosition,
      false,
      &caretX,
      &caretY,
      &hitTestMetrics
   );

   // The caret position itself is always the leading edge.
   // An additional offset indicates a trailing edge when non-zero.
   // This offset comes from the number of code-units in the
   // selected cluster or surrogate pair.
   m_caretPosition = hitTestMetrics.textPosition;
   m_caretPositionOffset = (isTrailingHit) ? hitTestMetrics.length : 0;

   // For invisible, zero-width characters (like line breaks
   // and formatting characters), force leading edge of the
   // next position.
   if(skipZeroWidth && hitTestMetrics.width == 0)
   {
      m_caretPosition += m_caretPositionOffset;
      m_caretPositionOffset = 0;
   }

}

HRESULT CDWriteTab::Initialize(HWND hwndParent, LPCTSTR tszText /*= __nullptr*/)
{
   HRESULT hResult;
   m_rcClientF = D2D1_RECT_F();
   if(__nullptr != tszText)
   {
      m_sInitText.Format(_T("%s"), tszText);
   }

   HWND hwndThis = Create(hwndParent, CWindow::rcDefault);
   hResult = (0 != hwndThis) ? S_OK : E_FAIL;
   if(SUCCEEDED(hResult))
   {
      m_hWndParent = hwndParent;
      hResult = CreateDeviceIndependentResources();
   }

   return hResult;
}

VOID CDWriteTab::ConstrainViewOrigin()
{
   // Keep the page on-screen by not allowing the origin
   // to go outside the page bounds.

   D2D1_POINT_2F pageSize = GetPageSize(m_CComPtrDWriteTextLayout0);

   if(m_OriginX > pageSize.x)
      m_OriginX = pageSize.x;
   if(m_OriginX < 0)
      m_OriginX = 0;

   if(m_OriginY > pageSize.y)
      m_OriginY = pageSize.y;
   if(m_OriginY < 0)
      m_OriginY = 0;
}

VOID CDWriteTab::CopyToClipboard()
{
   // Copies selected text to clipboard.

   DWRITE_TEXT_RANGE selectionRange = GetSelectionRange();
   if(selectionRange.length <= 0)
      return;

   // Open and empty existing contents.
   if(OpenClipboard())
   {
      if(EmptyClipboard())
      {
         // Allocate room for the text
         size_t byteSize = sizeof(wchar_t) * (selectionRange.length + 1);
         HGLOBAL hClipboardData = ::GlobalAlloc(GMEM_DDESHARE | GMEM_ZEROINIT, byteSize);

         if(hClipboardData != NULL)
         {
            void* memory = ::GlobalLock(hClipboardData);  // [byteSize] in bytes

            if(memory != NULL)
            {
               // Copy text to memory block.
               const wchar_t* text = m_sUnicode.GetString();
               memcpy(memory, &text[selectionRange.startPosition], byteSize);
               ::GlobalUnlock(hClipboardData);

               if(SetClipboardData(CF_UNICODETEXT, hClipboardData) != NULL)
               {
                  hClipboardData = NULL; // system now owns the clipboard, so don't touch it.
               }
            }
            ::GlobalFree(hClipboardData); // free if failed
         }
      }
      CloseClipboard();
   }
}

HRESULT CDWriteTab::CreateDeviceIndependentResources()
{
   m_CComPtrD2DFactory1.Release();
   HRESULT hResult = ::D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_CComPtrD2DFactory1);
   if(SUCCEEDED(hResult))
   {
      m_CComPtrDWriteFactory1.Release();
      hResult = ::DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory1),
         reinterpret_cast<IUnknown**>(&m_CComPtrDWriteFactory1));

   }
   if(SUCCEEDED(hResult))
   {//46.0 fills the 8.5 inch page with the row of '='s
      m_layoutEditor.SetFactory(m_CComPtrDWriteFactory1);
      hResult = m_CComPtrDWriteFactory1->CreateTextFormat(L"Gabriola", __nullptr, DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 18.0f, L"en-US", &m_CComPtrDWriteTextFormat0);
   }
   if(SUCCEEDED(hResult))
   {
      hResult = m_CComPtrDWriteTextFormat0->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
   }
   if(SUCCEEDED(hResult))
   {
      hResult = m_CComPtrDWriteTextFormat0->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
   }
   if(SUCCEEDED(hResult))
   {
      float fTabs = m_CComPtrDWriteTextFormat0->GetIncrementalTabStop();
      fTabs /= 4.f;
      m_CComPtrDWriteTextFormat0->SetIncrementalTabStop(fTabs);
   }

   float dpiX, dpiY;
   UINT uiDPI = ::GetDpiForWindow(this->m_hWnd);
#pragma warning(push)
#pragma warning(disable:4244)
   dpiX = dpiY = uiDPI;
#pragma warning(pop)
   m_dpiScaleX = dpiX / 96.0f;
   m_dpiScaleY = dpiY / 96.0f;


   if(SUCCEEDED(hResult))
   {
      auto fileData = DX::ReadDataAsync("ReadMe.txt");
      if(0 < fileData->Length)
      {
         int iUTF = IsUnicode(fileData);
         if(0 == iUTF)
         {
            m_sAnsi.Empty();
            m_sAnsi.Append(LPCSTR(fileData->Data), fileData->Length);
            m_sUnicode.AppendFormat(L"%S", (LPCSTR)m_sAnsi);
         }
         else if(8 == iUTF)
         {
            m_sAnsi.Empty();
            m_sAnsi.Append(LPCSTR(&fileData->Data[3]), (fileData->Length - 3));
            m_sUnicode.AppendFormat(L"%S", (LPCSTR)m_sAnsi);
         }
         else if(16 == iUTF)
         {
            m_sUnicode.Empty();
            m_sUnicode.Append(LPCWSTR(fileData->Data), (fileData->Length >> 1));
         }
         else if(17 == iUTF)// Big-Endian
         {
            m_sUnicode.Empty();
            if(0 == (fileData->Length % 2))
            {
               for(UINT i = 1; i < fileData->Length; i += 2)
               {
                  byte least = fileData->Data[i - 1];
                  fileData->Data[i - 1] = fileData->Data[i];
                  fileData->Data[i] = least;
               }
            }
            m_sUnicode.Append(LPCWSTR(fileData->Data), (fileData->Length >> 1));
         }
      }
      else
      {
         m_sUnicode.Append(_T("\nReadMe.txt could not be loaded."));
      }
   }
   if(SUCCEEDED(hResult))
   {
      float fX = 8.5f * dpiX;
      float fY = 11.0f * dpiY;
      hResult = m_CComPtrDWriteFactory1->CreateTextLayout(m_sUnicode, m_sUnicode.GetLength(), m_CComPtrDWriteTextFormat0, fX, fY, &m_CComPtrDWriteTextLayout0);
      if(SUCCEEDED(hResult))
      {
         fX = m_CComPtrDWriteTextLayout0->GetMaxWidth();
         fY = m_CComPtrDWriteTextLayout0->GetMaxHeight();
         m_OriginX = fX / 2.0f;
         m_OriginY = fY / 2.0f;
      }
   }

   HRESULT hr = m_CComPtrPageBackgroundEffect.CoCreateInstance(CLSID_DrawingEffect, __nullptr, CLSCTX_INPROC_SERVER);
   if(SUCCEEDED(hr))
   {
      hr = m_CComPtrPageBackgroundEffect->CreateEffect(0xFF000000 | D2D1::ColorF::White);
   }

   hr = m_CComPtrTextSelectionEffect.CoCreateInstance(CLSID_DrawingEffect, __nullptr, CLSCTX_INPROC_SERVER);
   if(SUCCEEDED(hr))
   {
      hr = m_CComPtrTextSelectionEffect->CreateEffect(0xFF000000 | D2D1::ColorF::LightSkyBlue);
   }

   hr = m_CComPtrImageSelectionEffect.CoCreateInstance(CLSID_DrawingEffect, __nullptr, CLSCTX_INPROC_SERVER);
   if(SUCCEEDED(hr))
   {
      hr = m_CComPtrImageSelectionEffect->CreateEffect(0x80000000 | D2D1::ColorF::LightSkyBlue);
   }

   hr = m_CComPtrCaretBackgroundEffect.CoCreateInstance(CLSID_DrawingEffect, __nullptr, CLSCTX_INPROC_SERVER);
   if(SUCCEEDED(hr))
   {
      hr = m_CComPtrCaretBackgroundEffect->CreateEffect(0xFF000000 | D2D1::ColorF::Black);
   }

   hr = m_CComPtrReuseableEffect.CoCreateInstance(CLSID_DrawingEffect, __nullptr, CLSCTX_INPROC_SERVER);
   if(SUCCEEDED(hr))
   {
      hr = m_CComPtrReuseableEffect->CreateEffect(0xFF000000 | D2D1::ColorF::Black);
   }

   if(SUCCEEDED(hResult))
   {
      hResult = m_CComPtrDWriteTextLayout0->SetDrawingEffect(m_CComPtrReuseableEffect, MakeDWriteTextRange(0));
   }

   if(SUCCEEDED(hResult))
   {
      hResult = m_CComPtrIWICImagingFactory.CoCreateInstance(CLSID_WICImagingFactory, __nullptr, CLSCTX_INPROC_SERVER);
      if(SUCCEEDED(hResult))
      {
         CComPtr<IWICBitmapSource> CComPtrIWICBitmapSource;//Changed below for debugging not using member BitmapSource
         hr = DX::LoadImageFromResource(L"InlineObjects", L"Image", m_CComPtrIWICImagingFactory, &CComPtrIWICBitmapSource);
         if(SUCCEEDED(hr))
         {
            CComPtr<IInlineImage> CComPtrIInlineImage0;
            hr = CComPtrIInlineImage0.CoCreateInstance(CLSID_InlineImage, __nullptr, CLSCTX_INPROC_SERVER);
            if(SUCCEEDED(hr))
            {
               hr = CComPtrIInlineImage0->SetImage(CComPtrIWICBitmapSource, 0);
               if(SUCCEEDED(hr))
               {
                  hr = m_CComPtrDWriteTextLayout0->SetInlineObject(CComPtrIInlineImage0, MakeDWriteTextRange(0, 1));
               }
            }
         }
         if(SUCCEEDED(hr))
         {
            CComPtr<IInlineImage> CComPtrIInlineImage0;
            hr = CComPtrIInlineImage0.CoCreateInstance(CLSID_InlineImage, __nullptr, CLSCTX_INPROC_SERVER);
            if(SUCCEEDED(hr))
            {
               hr = CComPtrIInlineImage0->SetImage(CComPtrIWICBitmapSource, 1);
               if(SUCCEEDED(hr))
               {
                  hr = m_CComPtrDWriteTextLayout0->SetInlineObject(CComPtrIInlineImage0, MakeDWriteTextRange(1, 1));
               }
            }
         }
      }
   }

   return hResult;
}

int CDWriteTab::IsUnicode(Platform::Array<byte>^ fileData)
{
   int iUTF = 0;
   if(__nullptr != fileData && 3 < fileData->Length)
   {
      PUSHORT pFirstWord = PUSHORT(&fileData->Data[0]);
      PUSHORT pSecondWord = PUSHORT(&fileData->Data[2]);
      if(0xFEFF == *pFirstWord)
      {//UTF16
         iUTF = 16;
      }
      else if(0xFFFE == *pFirstWord)
      {//UTF16 Big-Endian
         iUTF = 17;
      }
      else if((0xFFEE == *pFirstWord && 0x0000 == *pSecondWord)
         || (0x0000 == *pFirstWord && 0xFEFF == *pSecondWord))
      {//UTF32
         iUTF = 32;
      }
      else if(0xBBEF == *pFirstWord)//0xEFBB
      {
         USHORT usCheck = (*pSecondWord & 0x00FF);//0xFF00
         if(0x00BF == usCheck)//0xBF00
         {//UTF8
            iUTF = 8;
         }
      }
   }

   return m_iUTF = iUTF;
}

HRESULT CDWriteTab::CreateDeviceResources()
{
   HRESULT hResult = S_OK;
   //Create an IRenderTarget
   RECT clientRect;
   GetClientRect(&clientRect);
   D2D1_SIZE_U size = D2D1::SizeU(clientRect.right - clientRect.left, clientRect.bottom - clientRect.top);
   hResult = m_CComPtrRenderTarget.CoCreateInstance(CLSID_RenderTarget, __nullptr, CLSCTX_INPROC_SERVER);
   if(SUCCEEDED(hResult))
   {
      hResult = m_CComPtrRenderTarget->CreateTarget(m_CComPtrD2DFactory1, m_CComPtrDWriteFactory1, m_hWnd);
   }
   return hResult;
}

VOID CDWriteTab::DeleteSelection()
{// Deletes selection.
   DWRITE_TEXT_RANGE selectionRange = GetSelectionRange();
   if(0 < selectionRange.length)
   {
      UINT32 currentPos = selectionRange.startPosition;
      UINT32 endPos = currentPos + selectionRange.length;
      while(currentPos < endPos)
      {
         UINT32 rangeLength = CalculateRangeLengthAt(m_CComPtrDWriteTextLayout0, currentPos);
         if(0 == rangeLength)
         {
            break;
         }
         CComPtr<IDWriteInlineObject> CComPtrIDWriteInlineObject;
         HRESULT hr = m_CComPtrDWriteTextLayout0->GetInlineObject(currentPos, &CComPtrIDWriteInlineObject);
         if(SUCCEEDED(hr))
         {
            hr = m_CComPtrRenderTarget->RemoveInlineObject(CComPtrIDWriteInlineObject);
         }
         currentPos += rangeLength;
      }
      m_layoutEditor.RemoveTextAt(m_CComPtrDWriteTextLayout0, m_sUnicode, selectionRange.startPosition, selectionRange.length);
      SetSelection(SetSelectionModeAbsoluteLeading, selectionRange.startPosition, false);
      RefreshView();
   }
   else
   {
      CComPtr<IDWriteInlineObject> CComPtrIDWriteInlineObject;
      HRESULT hr = m_CComPtrDWriteTextLayout0->GetInlineObject(selectionRange.startPosition, &CComPtrIDWriteInlineObject);
      if(SUCCEEDED(hr))
      {
         hr = m_CComPtrRenderTarget->RemoveInlineObject(CComPtrIDWriteInlineObject);
      }
   }
}

HRESULT CDWriteTab::DrawPage(IRenderTarget* pIRenderTarget)
{
   HRESULT hr = S_OK;
   // Calculate actual location in render target based on the
   // current page transform and location of edit control.
   D2D1::Matrix3x2F pageTransform;
   GetViewMatrix(&DX::Cast(pageTransform));
   // Scale/Rotate canvas as needed
   D2D1_MATRIX_3X2_F previousTransform;
   m_CComPtrRenderTarget->GetTransform(previousTransform);
   m_CComPtrRenderTarget->SetTransform(pageTransform);
   // Draw the page
   D2D1_POINT_2F pageSize = GetPageSize(m_CComPtrDWriteTextLayout0);
   D2D1_RECT_F pageRect = { 0, 0, pageSize.x, pageSize.y };

   m_CComPtrRenderTarget->FillRectangle(pageRect, m_CComPtrPageBackgroundEffect);
   // Determine actual number of hit-test ranges
   DWRITE_TEXT_RANGE caretRange = GetSelectionRange();
   UINT32 actualHitTestCount = 0;

   if(caretRange.length > 0)
   {
      m_CComPtrDWriteTextLayout0->HitTestTextRange(
         caretRange.startPosition,
         caretRange.length,
         0, // x
         0, // y
         NULL,
         0, // metrics count
         &actualHitTestCount
      );
   }

   // Allocate enough room to return all hit-test metrics.
   std::vector<DWRITE_HIT_TEST_METRICS> hitTestMetrics(actualHitTestCount);

   if(caretRange.length > 0)
   {
      m_CComPtrDWriteTextLayout0->HitTestTextRange(
         caretRange.startPosition,
         caretRange.length,
         0, // x
         0, // y
         &hitTestMetrics[0],
         static_cast<UINT32>(hitTestMetrics.size()),
         &actualHitTestCount
      );
   }

   // Draw the selection ranges behind the text.
   if(actualHitTestCount > 0)
   {
      // Note that an ideal layout will return fractional values,
      // so you may see slivers between the selection ranges due
      // to the per-primitive antialiasing of the edges unless
      // it is disabled (better for performance anyway).
      m_CComPtrRenderTarget->SetAntialiasing(false);

      for(size_t i = 0; i < actualHitTestCount; ++i)
      {
         const DWRITE_HIT_TEST_METRICS& htm = hitTestMetrics[i];
         D2D1_RECT_F  highlightRect = {
            htm.left,
            htm.top,
            (htm.left + htm.width),
            (htm.top + htm.height)
         };

         m_CComPtrRenderTarget->FillRectangle(highlightRect, m_CComPtrTextSelectionEffect);
      }

      m_CComPtrRenderTarget->SetAntialiasing(true);
   }
   // Draw our caret onto the render target.
   D2D1_RECT_F caretRect;
   GetCaretRect(caretRect);
   m_CComPtrRenderTarget->SetAntialiasing(false);
   m_CComPtrRenderTarget->FillRectangle(caretRect, m_CComPtrCaretBackgroundEffect);
   m_CComPtrRenderTarget->SetAntialiasing(true);

   // Draw text
   m_CComPtrRenderTarget->DrawTextLayout(m_CComPtrDWriteTextLayout0, pageRect);

   // Draw the selection ranges in front of images.
   // This shades otherwise opaque images so they are visibly selected,
   // checking the isText field of the hit-test metrics.
   if(actualHitTestCount > 0)
   {
      // Note that an ideal layout will return fractional values,
      // so you may see slivers between the selection ranges due
      // to the per-primitive antialiasing of the edges unless
      // it is disabled (better for performance anyway).
      m_CComPtrRenderTarget->SetAntialiasing(false);

      for(size_t i = 0; i < actualHitTestCount; ++i)
      {
         const DWRITE_HIT_TEST_METRICS& htm = hitTestMetrics[i];
         if(htm.isText)
            continue; // Only draw selection if not text.

         D2D1_RECT_F highlightRect = {
            htm.left,
            htm.top,
            (htm.left + htm.width),
            (htm.top + htm.height)
         };

         m_CComPtrRenderTarget->FillRectangle(highlightRect, m_CComPtrImageSelectionEffect);//.Get()
      }

      m_CComPtrRenderTarget->SetAntialiasing(true);
   }

   // Restore transform
   m_CComPtrRenderTarget->SetTransform(previousTransform);
   return hr;
}

D2D1_POINT_2F CDWriteTab::GetPageSize(IDWriteTextLayout* pTextLayout)
{
   D2D1_POINT_2F pageSize = { 0 };
   if(__nullptr != pTextLayout)
   {
      DWRITE_TEXT_METRICS textMetrics;
      if(SUCCEEDED(pTextLayout->GetMetrics(&textMetrics)))
      {
         float width = max(textMetrics.layoutWidth, textMetrics.left + textMetrics.width);
         float height = max(textMetrics.layoutHeight, textMetrics.height);
         pageSize.x = width;
         pageSize.y = height;
      }
   }

   return pageSize;
}

VOID CDWriteTab::UpdateScrollInfo()
{
   if(__nullptr == m_CComPtrDWriteTextLayout0)
   {
      return;
   }

   // Determine scroll bar's step size in pixels by multiplying client rect by current view.
   RECT clientRect;
   GetClientRect(&clientRect);

   D2D1::Matrix3x2F pageTransform;
   GetInverseViewMatrix(&DX::Cast(pageTransform));

   // Transform vector of viewport size
   D2D1_POINT_2F clientSize = { float(clientRect.right), float(clientRect.bottom) };
   D2D1_POINT_2F scaledSize = { clientSize.x * pageTransform._11 + clientSize.y * pageTransform._21,
      clientSize.x * pageTransform._12 + clientSize.y * pageTransform._22 };

   float x = m_OriginX;
   float y = m_OriginY;
   D2D1_POINT_2F pageSize = DX::GetPageSize(m_CComPtrDWriteTextLayout0);

   SCROLLINFO scrollInfo = { sizeof(scrollInfo) };
   scrollInfo.fMask = SIF_PAGE | SIF_POS | SIF_RANGE;

   if(DX::IsLandscapeAngle(m_Angle))
   {
      std::swap(x, y);
      std::swap(pageSize.x, pageSize.y);
      std::swap(scaledSize.x, scaledSize.y);
   }

   // Set vertical scroll bar.
   scrollInfo.nPage = int(abs(scaledSize.y));
   scrollInfo.nPos = int(scaledSize.y >= 0 ? y : pageSize.y - y);
   scrollInfo.nMin = 0;
   scrollInfo.nMax = int(pageSize.y) + scrollInfo.nPage;
   SetScrollInfo(SB_VERT, &scrollInfo, TRUE);
   scrollInfo.nPos = 0;
   scrollInfo.nMax = 0;
   GetScrollInfo(SB_VERT, &scrollInfo);

   // Set horizontal scroll bar.
   scrollInfo.nPage = int(abs(scaledSize.x));
   scrollInfo.nPos = int(scaledSize.x >= 0 ? x : pageSize.x - x);
   scrollInfo.nMin = 0;
   scrollInfo.nMax = int(pageSize.x) + scrollInfo.nPage;
   SetScrollInfo(SB_HORZ, &scrollInfo, TRUE);
}

VOID CDWriteTab::RefreshView()
{
   UpdateScrollInfo();
   //PostRedraw();
}

// Gets the current caret position (in untransformed space).
VOID CDWriteTab::GetCaretRect(D2D1_RECT_F& caretRect)
{
   D2D1_RECT_F zeroRect = { 0 };
   caretRect = zeroRect;

   if(__nullptr == m_CComPtrDWriteTextLayout0)
   {
      return;
   }

   // Translate text character offset to point x,y.
   DWRITE_HIT_TEST_METRICS caretMetrics;
   float caretX, caretY;

   m_CComPtrDWriteTextLayout0->HitTestTextPosition(m_caretPosition,
      m_caretPositionOffset > 0, // trailing if nonzero, else leading edge
      &caretX, &caretY, &caretMetrics);

   // If a selection exists, draw the caret using the
   // line size rather than the font size.
   DWRITE_TEXT_RANGE selectionRange = GetSelectionRange();
   if(selectionRange.length > 0)
   {
      UINT32 actualHitTestCount = 1;
      m_CComPtrDWriteTextLayout0->HitTestTextRange(m_caretPosition,
         0, // length
         0, // x
         0, // y
         &caretMetrics, 1, &actualHitTestCount);

      caretY = caretMetrics.top;
   }

   // The default thickness of 1 pixel is almost _too_ thin on modern large monitors,
   // but we'll use it.
   DWORD caretIntThickness = 2;
   SystemParametersInfo(SPI_GETCARETWIDTH, 0, &caretIntThickness, FALSE);
   const float caretThickness = float(caretIntThickness);

   // Return the caret rect, untransformed.
   caretRect.left = caretX - caretThickness / 2.0f;
   caretRect.right = caretRect.left + caretThickness;
   caretRect.top = caretY;
   caretRect.bottom = caretY + caretMetrics.height;
}

void CDWriteTab::GetLineFromPosition(const DWRITE_LINE_METRICS* lineMetrics, // [lineCount]
   UINT32 lineCount, UINT32 textPosition, OUT UINT32* lineOut, OUT UINT32* linePositionOut)
{
   // Given the line metrics, determines the current line and starting text
   // position of that line by summing up the lengths. When the starting
   // line position is beyond the given text position, we have our line.

   UINT32 line = 0;
   UINT32 linePosition = 0;
   UINT32 nextLinePosition = 0;
   for(; line < lineCount; ++line)
   {
      linePosition = nextLinePosition;
      nextLinePosition = linePosition + lineMetrics[line].length;
      if(nextLinePosition > textPosition)
      {
         // The next line is beyond the desired text position,
         // so it must be in the current line.
         break;
      }
   }
   *linePositionOut = linePosition;
   *lineOut = min(line, lineCount - 1);
   return;
}

VOID CDWriteTab::GetLineMetrics(OUT std::vector<DWRITE_LINE_METRICS>& lineMetrics)
{
   // Retrieves the line metrics, used for caret navigation, up/down and home/end.
   DWRITE_TEXT_METRICS textMetrics;
   m_CComPtrDWriteTextLayout0->GetMetrics(&textMetrics);

   lineMetrics.resize(textMetrics.lineCount);
   m_CComPtrDWriteTextLayout0->GetLineMetrics(&lineMetrics.front(), textMetrics.lineCount, &textMetrics.lineCount);
}

DWRITE_TEXT_RANGE CDWriteTab::GetSelectionRange()
{
   // Returns a valid range of the current selection,
   // regardless of whether the caret or anchor is first.

   UINT32 caretBegin = m_caretAnchor;
   UINT32 caretEnd = m_caretPosition + m_caretPositionOffset;
   if(caretBegin > caretEnd)
      std::swap(caretBegin, caretEnd);

   // Limit to actual text length.
   UINT32 textLength = static_cast<UINT32>(m_sUnicode.GetLength());
   caretBegin = min(caretBegin, textLength);
   caretEnd = min(caretEnd, textLength);

   DWRITE_TEXT_RANGE textRange = { caretBegin, caretEnd - caretBegin };
   return textRange;
}

void  CDWriteTab::GetViewMatrix(OUT DWRITE_MATRIX* matrix) const
{    // Generates a view matrix from the current origin, angle, and scale.

   // Need the editor size for centering.
   RECT rect;
   GetClientRect(&rect);

   // Translate the origin to 0,0
   DWRITE_MATRIX translationMatrix =
   {
      1, 0,
      0, 1,
      -m_OriginX, -m_OriginY
   };

   // Scale and rotate
   double radians = XMConvertToRadians(fmod(m_Angle, 360.0f));
   double cosValue = cos(radians);
   double sinValue = sin(radians);

   // If rotation is a quarter multiple, ensure sin and cos are exactly one of {-1,0,1}
   if(fmod(m_Angle, 90.0f) == 0)
   {
      cosValue = floor(cosValue + .5);
      sinValue = floor(sinValue + .5);
   }

   DWRITE_MATRIX rotationMatrix = {
      float(cosValue * m_viewScaleX), float(sinValue * m_viewScaleX),
      float(-sinValue * m_viewScaleY), float(cosValue * m_viewScaleY),
      0, 0
   };

   // Set the origin in the center of the window
   float centeringFactor = .5f;
   DWRITE_MATRIX centerMatrix = {
      1, 0,
      0, 1,
      floor(float(rect.right * centeringFactor)), floor(float(rect.bottom * centeringFactor))
   };

   D2D1::Matrix3x2F resultA, resultB;

   resultB.SetProduct(DX::Cast(translationMatrix), DX::Cast(rotationMatrix));
   resultA.SetProduct(resultB, DX::Cast(centerMatrix));

   // For better pixel alignment (less blurry text)
   resultA._31 = floor(resultA._31);
   resultA._32 = floor(resultA._32);

   *matrix = *reinterpret_cast<DWRITE_MATRIX*>(&resultA);
}

void CDWriteTab::GetInverseViewMatrix(OUT DWRITE_MATRIX* matrix) const
{
   DWRITE_MATRIX viewMatrix;
   GetViewMatrix(&viewMatrix);
   DX::ComputeInverseMatrix(viewMatrix, *matrix);
}

HRESULT CDWriteTab::InsertText(LPCWSTR wszText, UINT32 cch)//WCHAR const*
{
   UINT32 absolutePosition = m_caretPosition + m_caretPositionOffset;
   return m_layoutEditor.InsertTextAt(m_CComPtrDWriteTextLayout0, m_sUnicode, absolutePosition, wszText, cch, &m_caretFormat);
}

VOID CDWriteTab::MirrorXCoordinate(IN OUT float& x)
{
   // On RTL builds, coordinates may need to be restored to or converted
   // from Cartesian coordinates, where x increases positively to the right.
   if(GetWindowLong(GWL_EXSTYLE) & WS_EX_LAYOUTRTL)
   {
      RECT rect;
      GetClientRect(&rect);
      x = float(rect.right) - x - 1;
   }
}

HRESULT CDWriteTab::OpenImageFile(LPCWSTR wszPath)
{
   HRESULT hr;
   // Create an inline object, using WIC to load the image
   CComPtr<IWICBitmapSource> CComPtrIWICBitmapSource;
   hr = DX::LoadImageFromFile(wszPath, m_CComPtrIWICImagingFactory, &CComPtrIWICBitmapSource);
   // Get the range of text to be replaced with an inline image.
   // If no text is selected, insert the Unicode object replacement
   // character as an anchor.
   if(SUCCEEDED(hr))
   {
      CComPtr<IInlineImage> CComPtrIInlineImage;
      hr = CComPtrIInlineImage.CoCreateInstance(CLSID_InlineImage, __nullptr, CLSCTX_INPROC_SERVER);
      if(SUCCEEDED(hr))
      {
         hr = CComPtrIInlineImage->SetImage(CComPtrIWICBitmapSource);
         if(SUCCEEDED(hr))
         {
            DWRITE_TEXT_RANGE textRange = GetSelectionRange();
            if(textRange.length <= 0)
            {
               textRange.length = 1;
               InsertText(L"\xFFFC", 1);
               SetSelection(SetSelectionModeAbsoluteLeading, textRange.startPosition, false, false);
               SetSelection(SetSelectionModeAbsoluteLeading, textRange.startPosition + 1, true, false);
            }

            hr = m_CComPtrDWriteTextLayout0->SetInlineObject(CComPtrIInlineImage, textRange);
            //CComPtrIInlineImage.Detach();
            SetSelection(SetSelectionModeEnd, 0, false, true);
            UINT32 absolutePosition = m_caretPosition + m_caretPositionOffset;
            m_layoutEditor.InsertTextAt(m_CComPtrDWriteTextLayout0, m_sUnicode, absolutePosition, L"\r\n", 2, &m_caretFormat);
            SetSelection(SetSelectionModeAbsoluteLeading, absolutePosition + 2, false, false);
            RefreshView();
         }
      }
      //CComPtrIWICBitmapSource.Detach();
   }
   return hr;
}

VOID CDWriteTab::PasteFromClipboard()
{
   // Pastes text from clipboard at current caret position.

   DeleteSelection();

   UINT32 characterCount = 0;

   // Copy Unicode text from clipboard.

   if(OpenClipboard())
   {
      HGLOBAL hClipboardData = GetClipboardData(CF_UNICODETEXT);

      if(hClipboardData != NULL)
      {
         // Get text and size of text.
         size_t byteSize = GlobalSize(hClipboardData);
         void* memory = GlobalLock(hClipboardData); // [byteSize] in bytes
         const wchar_t* text = reinterpret_cast<const wchar_t*>(memory);
         if(__nullptr != text)
         {
            characterCount = static_cast<UINT32>(wcsnlen(text, byteSize / sizeof(wchar_t)));

            if(NULL != memory)
            {
               // Insert the text at the current position.
               m_layoutEditor.InsertTextAt(
                  m_CComPtrDWriteTextLayout0,
                  m_sUnicode,
                  m_caretPosition + m_caretPositionOffset,
                  text,
                  characterCount
               );
            }
         }
         ::GlobalUnlock(hClipboardData);
      }
      CloseClipboard();
   }

   SetSelection(SetSelectionModeRightChar, characterCount, true);
   RefreshView();
}
// Resets the default view
void CDWriteTab::ResetView()
{
   float fX = m_CComPtrDWriteTextLayout0->GetMaxWidth();
   float fY = m_CComPtrDWriteTextLayout0->GetMaxHeight();
   m_OriginX = fX / 2.f;
   m_OriginY = fY / 2.f;
   m_viewScaleX = m_viewScaleY = 1;
   m_Angle = 0;

   RefreshView();
}

VOID CDWriteTab::SetScale(float scaleX, float scaleY, bool bRelative)
{
   if(bRelative)
   {
      m_viewScaleX *= scaleX;
      m_viewScaleY *= scaleY;
   }
   else
   {
      m_viewScaleX = scaleX;
      m_viewScaleY = scaleY;
   }
   RefreshView();
}

bool CDWriteTab::SetSelection(SetSelectionMode moveMode, UINT32 advance, bool extendSelection, bool updateCaretFormat)
{    // Moves the caret relatively or absolutely, optionally extending the
   // selection range (for example, when shift is held).

   UINT32 line = UINT32_MAX; // current line number, needed by a few modes
   UINT32 absolutePosition = m_caretPosition + m_caretPositionOffset;
   UINT32 oldAbsolutePosition = absolutePosition;
   UINT32 oldCaretAnchor = m_caretAnchor;

   switch(moveMode)
   {
   case SetSelectionModeLeft:
      m_caretPosition += m_caretPositionOffset;
      if(m_caretPosition > 0)
      {
         --m_caretPosition;
         AlignCaretToNearestCluster(false, true);

         // special check for CR/LF pair
         absolutePosition = m_caretPosition + m_caretPositionOffset;
         if(absolutePosition >= 1
            && signed(absolutePosition) < m_sUnicode.GetAllocLength()
            && m_sUnicode[absolutePosition - 1] == '\r'
            && m_sUnicode[absolutePosition] == '\n')
         {
            m_caretPosition = absolutePosition - 1;
            AlignCaretToNearestCluster(false, true);
         }
      }
      break;

   case SetSelectionModeRight:
      m_caretPosition = absolutePosition;
      AlignCaretToNearestCluster(true, true);

      // special check for CR/LF pair
      absolutePosition = m_caretPosition + m_caretPositionOffset;
      if(absolutePosition >= 1
         && signed(absolutePosition) < m_sUnicode.GetAllocLength()
         && m_sUnicode[absolutePosition - 1] == '\r'
         && m_sUnicode[absolutePosition] == '\n')
      {
         m_caretPosition = absolutePosition + 1;
         AlignCaretToNearestCluster(false, true);
      }
      break;

   case SetSelectionModeLeftChar:
      m_caretPosition = absolutePosition;
      m_caretPosition -= min(advance, absolutePosition);
      m_caretPositionOffset = 0;
      break;

   case SetSelectionModeRightChar:
      m_caretPosition = absolutePosition + advance;
      m_caretPositionOffset = 0;
      {
         // Use hit-testing to limit text position.
         DWRITE_HIT_TEST_METRICS hitTestMetrics;
         float caretX, caretY;

         m_CComPtrDWriteTextLayout0->HitTestTextPosition(
            m_caretPosition,
            false,
            &caretX,
            &caretY,
            &hitTestMetrics
         );
         m_caretPosition = min(m_caretPosition, hitTestMetrics.textPosition + hitTestMetrics.length);
      }
      break;

   case SetSelectionModeUp:
   case SetSelectionModeDown:
   {
      // Retrieve the line metrics to figure out what line we are on.
      std::vector<DWRITE_LINE_METRICS> lineMetrics;
      GetLineMetrics(lineMetrics);

      UINT32 linePosition;
      GetLineFromPosition(
         &lineMetrics.front(),
         static_cast<UINT32>(lineMetrics.size()),
         m_caretPosition,
         &line,
         &linePosition
      );

      // Move up a line or down
      if(moveMode == SetSelectionModeUp)
      {
         if(line <= 0)
            break; // already top line
         line--;
         linePosition -= lineMetrics[line].length;
      }
      else
      {
         linePosition += lineMetrics[line].length;
         line++;
         if(line >= lineMetrics.size())
            break; // already bottom line
      }

      // To move up or down, we need three hit-testing calls to determine:
      // 1. The x of where we currently are.
      // 2. The y of the new line.
      // 3. New text position from the determined x and y.
      // This is because the characters are variable size.

      DWRITE_HIT_TEST_METRICS hitTestMetrics;
      float caretX, caretY, dummyX;

      // Get x of current text position
      m_CComPtrDWriteTextLayout0->HitTestTextPosition(
         m_caretPosition,
         m_caretPositionOffset > 0, // trailing if nonzero, else leading edge
         &caretX,
         &caretY,
         &hitTestMetrics
      );

      // Get y of new position
      m_CComPtrDWriteTextLayout0->HitTestTextPosition(
         linePosition,
         false, // leading edge
         &dummyX,
         &caretY,
         &hitTestMetrics
      );

      // Now get text position of new x,y.
      BOOL isInside, isTrailingHit;
      m_CComPtrDWriteTextLayout0->HitTestPoint(
         caretX,
         caretY,
         &isTrailingHit,
         &isInside,
         &hitTestMetrics
      );

      m_caretPosition = hitTestMetrics.textPosition;
      m_caretPositionOffset = isTrailingHit ? (hitTestMetrics.length > 0) : 0;
   }
   break;

   case SetSelectionModeLeftWord:
   case SetSelectionModeRightWord:
   {
      // To navigate by whole words, we look for the canWrapLineAfter
      // flag in the cluster metrics.

      // First need to know how many clusters there are.
      std::vector<DWRITE_CLUSTER_METRICS> clusterMetrics;
      UINT32 clusterCount;
      m_CComPtrDWriteTextLayout0->GetClusterMetrics(NULL, 0, &clusterCount);

      if(clusterCount == 0)
         break;

      // Now we actually read them.
      clusterMetrics.resize(clusterCount);
      m_CComPtrDWriteTextLayout0->GetClusterMetrics(&clusterMetrics.front(), clusterCount, &clusterCount);

      m_caretPosition = absolutePosition;

      UINT32 clusterPosition = 0;
      UINT32 oldCaretPosition = m_caretPosition;

      if(moveMode == SetSelectionModeLeftWord)
      {
         // Read through the clusters, keeping track of the farthest valid
         // stopping point just before the old position.
         m_caretPosition = 0;
         m_caretPositionOffset = 0; // leading edge
         for(UINT32 cluster = 0; cluster < clusterCount; ++cluster)
         {
            clusterPosition += clusterMetrics[cluster].length;
            if(clusterMetrics[cluster].canWrapLineAfter)
            {
               if(clusterPosition >= oldCaretPosition)
                  break;

               // Update in case we pass this point next loop.
               m_caretPosition = clusterPosition;
            }
         }
      }
      else // SetSelectionModeRightWord
      {
         // Read through the clusters, looking for the first stopping point
         // after the old position.
         for(UINT32 cluster = 0; cluster < clusterCount; ++cluster)
         {
            UINT32 clusterLength = clusterMetrics[cluster].length;
            m_caretPosition = clusterPosition;
            m_caretPositionOffset = clusterLength; // trailing edge
            if(clusterPosition >= oldCaretPosition && clusterMetrics[cluster].canWrapLineAfter)
               break; // first stopping point after old position.

            clusterPosition += clusterLength;
         }
      }
   }
   break;

   case SetSelectionModeHome:
   case SetSelectionModeEnd:
   {
      // Retrieve the line metrics to know first and last position
      // on the current line.
      std::vector<DWRITE_LINE_METRICS> lineMetrics;
      GetLineMetrics(lineMetrics);

      GetLineFromPosition(
         &lineMetrics.front(),
         static_cast<UINT32>(lineMetrics.size()),
         m_caretPosition,
         &line,
         &m_caretPosition
      );

      m_caretPositionOffset = 0;
      if(moveMode == SetSelectionModeEnd)
      {
         // Place the caret at the last character on the line,
         // excluding line breaks. In the case of wrapped lines,
         // newlineLength will be 0.
         UINT32 lineLength = lineMetrics[line].length - lineMetrics[line].newlineLength;
         m_caretPositionOffset = min(lineLength, 1u);
         m_caretPosition += lineLength - m_caretPositionOffset;
         AlignCaretToNearestCluster(true);
      }
   }
   break;

   case SetSelectionModeFirst:
      m_caretPosition = 0;
      m_caretPositionOffset = 0;
      break;

   case SetSelectionModeAll:
      m_caretAnchor = 0;
      extendSelection = true;
      __fallthrough;

   case SetSelectionModeLast:
      m_caretPosition = UINT32_MAX;
      m_caretPositionOffset = 0;
      AlignCaretToNearestCluster(true);
      break;

   case SetSelectionModeAbsoluteLeading:
      m_caretPosition = advance;
      m_caretPositionOffset = 0;
      break;

   case SetSelectionModeAbsoluteTrailing:
      m_caretPosition = advance;
      AlignCaretToNearestCluster(true);
      break;
   }

   absolutePosition = m_caretPosition + m_caretPositionOffset;

   if(!extendSelection)
      m_caretAnchor = absolutePosition;

   bool caretMoved = (absolutePosition != oldAbsolutePosition)
      || (m_caretAnchor != oldCaretAnchor);

   if(caretMoved)
   {
      BOOL bHandled = TRUE;
      // update the caret formatting attributes
      if(updateCaretFormat)
         UpdateCaretFormatting();

      //PostRedraw();

      D2D1_RECT_F rcCaretF;
      DWRITE_MATRIX invMx;
      GetInverseViewMatrix(&invMx);
      GetCaretRect(rcCaretF);
      float mappedOriginX = (m_rcClientF.left * invMx.m11 + m_rcClientF.top * invMx.m21 + invMx.dx);
      float mappedOriginY = (m_rcClientF.left * invMx.m12 + m_rcClientF.top * invMx.m22 + invMx.dy);
      float mappedFarX = (m_rcClientF.right * invMx.m11 + m_rcClientF.bottom * invMx.m21 + invMx.dx);
      float mappedFarY = (m_rcClientF.right * invMx.m12 + m_rcClientF.bottom * invMx.m22 + invMx.dy);
      if(rcCaretF.top < mappedOriginY)
      {
         int deltaUp = int(ceil(max(rcCaretF.bottom - rcCaretF.top, mappedOriginY - rcCaretF.top)));
         OnScroll(WM_VSCROLL, SB_LINEUP, deltaUp, bHandled);
      }
      else if(mappedFarY < rcCaretF.bottom)
      {
         int deltaDown = int(ceil(max(rcCaretF.bottom - rcCaretF.top, rcCaretF.bottom - mappedFarY)));
         OnScroll(WM_VSCROLL, SB_LINEDOWN, deltaDown, bHandled);
      }
      if(rcCaretF.left < mappedOriginX)
      {
         int deltaLeft = int(ceil(mappedFarX - rcCaretF.left));
         OnScroll(WM_HSCROLL, SB_LINEUP, deltaLeft, bHandled);
      }
      else if(mappedFarX < rcCaretF.right)
      {
         int deltaRight = int(ceil(rcCaretF.right - mappedFarX));
         OnScroll(WM_HSCROLL, SB_LINEDOWN, deltaRight, bHandled);
      }
      UpdateSystemCaret(rcCaretF);
   }

   return caretMoved;
}

bool CDWriteTab::SetSelectionFromPoint(float x, float y, bool extendSelection)
{
   // Returns the text position corresponding to the mouse x,y.
   // If hitting the trailing side of a cluster, return the
   // leading edge of the following text position.

   BOOL isTrailingHit;
   BOOL isInside;
   DWRITE_HIT_TEST_METRICS caretMetrics;

   // Remap display coordinates to actual.
   DWRITE_MATRIX matrix;
   GetInverseViewMatrix(&matrix);

   float transformedX = (x * matrix.m11 + y * matrix.m21 + matrix.dx);
   float transformedY = (x * matrix.m12 + y * matrix.m22 + matrix.dy);

   m_CComPtrDWriteTextLayout0->HitTestPoint(
      transformedX,
      transformedY,
      &isTrailingHit,
      &isInside,
      &caretMetrics
   );

   // Update current selection according to click or mouse drag.
   SetSelection(
      isTrailingHit ? SetSelectionModeAbsoluteTrailing : SetSelectionModeAbsoluteLeading,
      caretMetrics.textPosition,
      extendSelection
   );

   return true;
}

VOID CDWriteTab::UpdateCaretFormatting()
{
   UINT32 currentPos = m_caretPosition + m_caretPositionOffset;

   if(currentPos > 0)
   {
      --currentPos; // Always adopt the trailing properties.
   }

   // Get the family name
   m_caretFormat.fontFamilyName[0] = '\0';
   m_CComPtrDWriteTextLayout0->GetFontFamilyName(currentPos, &m_caretFormat.fontFamilyName[0], ARRAYSIZE(m_caretFormat.fontFamilyName));

   // Get the locale
   m_caretFormat.localeName[0] = '\0';
   m_CComPtrDWriteTextLayout0->GetLocaleName(currentPos, &m_caretFormat.localeName[0], ARRAYSIZE(m_caretFormat.localeName));

   // Get the remaining attributes...
   m_CComPtrDWriteTextLayout0->GetFontWeight(currentPos, &m_caretFormat.fontWeight);
   m_CComPtrDWriteTextLayout0->GetFontStyle(currentPos, &m_caretFormat.fontStyle);
   m_CComPtrDWriteTextLayout0->GetFontStretch(currentPos, &m_caretFormat.fontStretch);
   m_CComPtrDWriteTextLayout0->GetFontSize(currentPos, &m_caretFormat.fontSize);
   m_CComPtrDWriteTextLayout0->GetUnderline(currentPos, &m_caretFormat.hasUnderline);
   m_CComPtrDWriteTextLayout0->GetStrikethrough(currentPos, &m_caretFormat.hasStrikethrough);

   // Get the current color.
   //IUnknown* drawingEffect = NULL;
   CComPtr<IDrawingEffect> ccpDrawingEffect;
   HRESULT hr = m_CComPtrDWriteTextLayout0->GetDrawingEffect(currentPos, (IUnknown**)&ccpDrawingEffect);
   m_caretFormat.color = 0;
   if(SUCCEEDED(hr))
   {
      if(__nullptr != ccpDrawingEffect)
      {
         ccpDrawingEffect->GetColor(m_caretFormat.color);
      }
   }
}

VOID CDWriteTab::UpdateSystemCaret(const D2D1_RECT_F& rect)
{    // Moves the system caret to a new position.

   // Although we don't actually use the system caret (drawing our own
   // instead), this is important for accessibility, so the magnifier
   // can follow text we type. The reason we draw our own directly
   // is because intermixing DirectX and GDI content (the caret) reduces
   // performance.

   // Gets the current caret position (in untransformed space).
   HWND hWnd = GetFocus();
   if(hWnd == m_hWnd/* || hWnd == m_hWndParent*/) // Only update if we have focus.
   {

      D2D1::Matrix3x2F pageTransform;
      GetViewMatrix(&DX::Cast(pageTransform));

      // Transform caret top/left and size according to current scale and origin.
      D2D1_POINT_2F caretPoint = pageTransform.TransformPoint(D2D1::Point2F(rect.left, rect.top));

      float width = (rect.right - rect.left);
      float height = (rect.bottom - rect.top);
      float transformedWidth = width * pageTransform._11 + height * pageTransform._21;
      float transformedHeight = width * pageTransform._12 + height * pageTransform._22;

      // Update the caret's location, rounding to nearest integer so that
      // it lines up with the text selection.

      int intX = DX::RoundToInt(caretPoint.x);
      int intY = DX::RoundToInt(caretPoint.y);
      int intWidth = DX::RoundToInt(transformedWidth);
      int intHeight = DX::RoundToInt(caretPoint.y + transformedHeight) - intY;

      ::CreateCaret(m_hWnd, __nullptr, intWidth, intHeight);
      ::SetCaretPos(intX, intY);
   }
   // Don't actually call ShowCaret. It's enough to just set its position.
}

LRESULT CDWriteTab::OnContextMenu(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
   LRESULT lResult = 0;
   POINT ptScreen;
   UINT idBegin = IDS_CONTEXT0, idEnd = IDS_CONTEXT6 + 1UL;//4 inclusive
   CStringW csItem;
   ptScreen.x = GET_X_LPARAM(lParam);
   ptScreen.y = GET_Y_LPARAM(lParam);
   if(-1L == ptScreen.x && -1L == ptScreen.y)
   {// App Key/Context Key / Shift F10
      GetCaretPos(&ptScreen);
   }
   HMENU hPopup = ::CreatePopupMenu();
   if(__nullptr != hPopup)
   {
      MENUITEMINFO menuItemInfo = { 0 };
      menuItemInfo.cbSize = sizeof(MENUITEMINFO);
      menuItemInfo.fMask = MIIM_TYPE | MIIM_ID;
      menuItemInfo.fType = MFT_STRING;
      for(UINT id = idBegin; id < idEnd; ++id)
      {
         menuItemInfo.wID = id;
         csItem.Format(id);
         menuItemInfo.dwTypeData = csItem.GetBuffer();
         menuItemInfo.cch = csItem.GetLength();
         ::InsertMenuItem(hPopup, menuItemInfo.wID, FALSE, &menuItemInfo);
         csItem.Empty();
      }
      int idCmd = ::TrackPopupMenuEx(hPopup, TPM_RIGHTALIGN | TPM_TOPALIGN | TPM_RETURNCMD, ptScreen.x, ptScreen.y, m_hWnd, __nullptr);
      ::DestroyMenu(hPopup);
      if(0L != idCmd && IDS_CONTEXT6 >= idCmd)
      {
         switch(idCmd)
         {
         case IDS_CONTEXT0:
            OnSetInlineImage(nMsg, wParam, lParam, bHandled);
            break;
         case IDS_CONTEXT5:
            m_Angle += 90;
            RefreshView();
            break;
         case IDS_CONTEXT6:
            m_Angle -= 90;
            RefreshView();
            break;
         }
      }
   }
   return lResult;
}

LRESULT CDWriteTab::OnCreate(UINT /*nMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
   LRESULT lResult = 0;
   UINT_PTR nID = SetTimer(0, 33, __nullptr);
   CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
   if(__nullptr != pCreate)
   {
      LPCTSTR tszText = (LPCTSTR)pCreate->lpCreateParams;
      if(__nullptr != tszText)
      {
         m_sCreateText.Format(_T("%s"), tszText);
      }
   }

   return lResult;
}

LRESULT CDWriteTab::OnDisplayChange(UINT /*nMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   InvalidateRect(__nullptr, TRUE);

   return 0;
}

LRESULT CDWriteTab::OnKeyCharacter(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
   // Inserts text characters.
   UINT32 charCode = static_cast<UINT32>(wParam);
   // Allow normal characters and tabs
   if(charCode >= 0x20 || charCode == 9)
   {
      // Replace any existing selection.
      DeleteSelection();

      // Convert the UTF32 character code from the Window message to UTF16,
      // yielding 1-2 code-units. Then advance the caret position by how
      // many code-units were inserted.

      UINT32 charsLength = 1;
      wchar_t chars[2] = { static_cast<wchar_t>(charCode), 0 };

      // If above the basic multi-lingual plane, split into
      // leading and trailing surrogates.
      if(charCode > 0xFFFF)
      {
         // From http://unicode.org/faq/utf_bom.html#35
         chars[0] = wchar_t(0xD800 + (charCode >> 10) - (0x10000 >> 10));
         chars[1] = wchar_t(0xDC00 + (charCode & 0x3FF));
         charsLength++;
      }
      m_layoutEditor.InsertTextAt(m_CComPtrDWriteTextLayout0, m_sUnicode, m_caretPosition + m_caretPositionOffset, chars, charsLength, &m_caretFormat);
      SetSelection(SetSelectionModeRight, charsLength, false, false);

      RefreshView();
   }
   return 0;
}

LRESULT CDWriteTab::OnKeyPress(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
   // Handles caret navigation and special presses that
   // do not generate characters.
   UINT32 keyCode = static_cast<UINT32>(wParam);
   bool heldShift = (GetKeyState(VK_SHIFT) & 0x80) != 0;
   bool heldControl = (GetKeyState(VK_CONTROL) & 0x80) != 0;

   UINT32 absolutePosition = m_caretPosition + m_caretPositionOffset;

   switch(keyCode)
   {
   case VK_RETURN:
      // Insert CR/LF pair
      DeleteSelection();
      m_layoutEditor.InsertTextAt(m_CComPtrDWriteTextLayout0, m_sUnicode, absolutePosition, L"\r\n", 2, &m_caretFormat);
      SetSelection(SetSelectionModeAbsoluteLeading, absolutePosition + 2, false, false);
      RefreshView();
      break;

   case VK_BACK:
      // Erase back one character (less than a character though).
      // Since layout's hit-testing always returns a whole cluster,
      // we do the surrogate pair detection here directly. Otherwise
      // there would be no way to delete just the diacritic following
      // a base character.

      if(absolutePosition != m_caretAnchor)
      {
         // delete the selected text
         DeleteSelection();
      }
      else if(absolutePosition > 0)
      {
         UINT32 count = 1;
         // Need special case for surrogate pairs and CR/LF pair.
         if(absolutePosition >= 2
            && signed(absolutePosition) <= m_sUnicode.GetAllocLength())
         {
            wchar_t charBackOne = m_sUnicode[absolutePosition - 1];
            wchar_t charBackTwo = m_sUnicode[absolutePosition - 2];
            if((DX::IsLowSurrogate(charBackOne) && DX::IsHighSurrogate(charBackTwo))
               || (charBackOne == '\n' && charBackTwo == '\r'))
            {
               count = 2;
            }
         }
         SetSelection(SetSelectionModeLeftChar, count, false);
         DeleteSelection();
         m_layoutEditor.RemoveTextAt(m_CComPtrDWriteTextLayout0, m_sUnicode, m_caretPosition, count);
         RefreshView();
      }
      break;

   case VK_DELETE:
      // Delete following cluster.

      if(absolutePosition != m_caretAnchor)
      {
         // Delete all the selected text.
         DeleteSelection();
      }
      else
      {
         DWRITE_HIT_TEST_METRICS hitTestMetrics;
         float caretX, caretY;

         // Get the size of the following cluster.
         m_CComPtrDWriteTextLayout0->HitTestTextPosition(
            absolutePosition,
            false,
            &caretX,
            &caretY,
            &hitTestMetrics
         );
         //SetSelection(SetSelectionModeRightChar, hitTestMetrics.length, false);
         DeleteSelection();
         m_layoutEditor.RemoveTextAt(m_CComPtrDWriteTextLayout0, m_sUnicode, hitTestMetrics.textPosition, hitTestMetrics.length);

         SetSelection(SetSelectionModeAbsoluteLeading, hitTestMetrics.textPosition, false);
         RefreshView();
      }
      break;

   case VK_TAB:
      break; // want tabs

   case VK_LEFT: // seek left one cluster
      SetSelection(heldControl ? SetSelectionModeLeftWord : SetSelectionModeLeft, 1, heldShift);
      break;

   case VK_RIGHT: // seek right one cluster
      SetSelection(heldControl ? SetSelectionModeRightWord : SetSelectionModeRight, 1, heldShift);
      break;

   case VK_UP: // up a line
      SetSelection(SetSelectionModeUp, 1, heldShift);
      break;

   case VK_DOWN: // down a line
      SetSelection(SetSelectionModeDown, 1, heldShift);
      break;

   case VK_HOME: // beginning of line
      SetSelection(heldControl ? SetSelectionModeFirst : SetSelectionModeHome, 0, heldShift);
      break;

   case VK_END: // end of line
      SetSelection(heldControl ? SetSelectionModeLast : SetSelectionModeEnd, 0, heldShift);
      break;

   case 'C':
      if(heldControl)
         CopyToClipboard();
      break;

   case VK_INSERT:
      if(heldControl)
         CopyToClipboard();
      else if(heldShift)
         PasteFromClipboard();
      break;

   case 'V':
      if(heldControl)
         PasteFromClipboard();
      break;

   case 'X':
      if(heldControl)
      {
         CopyToClipboard();
         DeleteSelection();
      }
      break;

   case 'A':
      if(heldControl)
         SetSelection(SetSelectionModeAll, 0, true);
      break;
   }
   return 0;
}

LRESULT CDWriteTab::OnKillFocus(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
   DestroyCaret();
   return 0;
}

LRESULT CDWriteTab::OnMouseExit(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
   m_currentlySelecting = false;
   m_currentlyPanning = false;
   return 0;
}

LRESULT CDWriteTab::OnMouseMove(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
   float x = float(GET_X_LPARAM(lParam));
   float y = float(GET_Y_LPARAM(lParam));
   // Selects text or pans.

   MirrorXCoordinate(x);

   if(m_currentlySelecting)
   {
      // Drag current selection.
      SetSelectionFromPoint(x, y, true);
   }
   else if(m_currentlyPanning)
   {
      DWRITE_MATRIX matrix;
      GetInverseViewMatrix(&matrix);

      float xDif = x - m_previousMouseX;
      float yDif = y - m_previousMouseY;
      m_previousMouseX = x;
      m_previousMouseY = y;

      m_OriginX -= (xDif * matrix.m11 + yDif * matrix.m21);
      m_OriginY -= (xDif * matrix.m12 + yDif * matrix.m22);
      ConstrainViewOrigin();

      RefreshView();
   }
   return 0;
}

LRESULT CDWriteTab::OnMousePress(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
   SetFocus();
   SetCapture();
   float x = float(GET_X_LPARAM(lParam));
   float y = float(GET_Y_LPARAM(lParam));
   MirrorXCoordinate(x);

   if(WM_LBUTTONDOWN == nMsg)
   {
      // Start dragging selection.
      m_currentlySelecting = true;

      bool heldShift = (GetKeyState(VK_SHIFT) & 0x80) != 0;
      SetSelectionFromPoint(x, y, heldShift);
   }
   else if(WM_MBUTTONDOWN == nMsg)
   {
      m_previousMouseX = x;
      m_previousMouseY = y;
      m_currentlyPanning = true;
   }
   return 0;
}

LRESULT CDWriteTab::OnMouseRelease(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
   ReleaseCapture();
   float x = float(GET_X_LPARAM(lParam));
   float y = float(GET_Y_LPARAM(lParam));
   MirrorXCoordinate(x);

   if(WM_LBUTTONUP == nMsg)
   {
      m_currentlySelecting = false;
   }
   else if(WM_MBUTTONUP == nMsg)
   {
      m_currentlyPanning = false;
   }
   else if(WM_RBUTTONUP == nMsg)
   {
      bHandled = false;
   }
   return 0;
}

LRESULT CDWriteTab::OnMouseScroll(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
   // Retrieve the lines-to-scroll or characters-to-scroll user setting,
   // using a default value if the API failed.
   UINT userSetting;
   BOOL success = SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &userSetting, 0);
   if(success == FALSE)
      userSetting = 1;

   // Set x,y scroll difference,
   // depending on whether horizontal or vertical scroll.
   float zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
   float yScroll = (zDelta / WHEEL_DELTA) * userSetting;
   float xScroll = 0;
   if(WM_MOUSEHWHEEL == nMsg)
   {
      xScroll = -yScroll;
      yScroll = 0;
   }

   // Pans or scales the editor.

   bool heldShift = (GetKeyState(VK_SHIFT) & 0x80) != 0;
   bool heldControl = (GetKeyState(VK_CONTROL) & 0x80) != 0;

   if(heldControl)
   {
      // Scale
      float scaleFactor = (yScroll > 0) ? 1.0625f : 1 / 1.0625f;
      SetScale(scaleFactor, scaleFactor, true);
   }
   else
   {
      // Pan
      DWRITE_MATRIX matrix;
      GetInverseViewMatrix(&matrix);

      yScroll *= MouseScrollFactor;
      xScroll *= MouseScrollFactor; // for mice that support horizontal panning
      if(heldShift)
         std::swap(xScroll, yScroll);

      m_OriginX -= (xScroll * matrix.m11 + yScroll * matrix.m21);
      m_OriginY -= (xScroll * matrix.m12 + yScroll * matrix.m22);
      ConstrainViewOrigin();

      RefreshView();
   }
   return 0;
}

LRESULT CDWriteTab::OnPaint(UINT /*nMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   PAINTSTRUCT ps;
   HRESULT hr;
   DWORD dwThreadID = GetCurrentThreadId();
   BeginPaint(&ps);
   if(__nullptr == m_CComPtrRenderTarget)
   {
      CreateDeviceResources();
   }

   if(__nullptr != m_CComPtrRenderTarget)
   {
      m_CComPtrRenderTarget->BeginDraw();
      m_CComPtrRenderTarget->Clear(D2D1::ColorF::LightGray);
      DrawPage(m_CComPtrRenderTarget);
      hr = m_CComPtrRenderTarget->EndDraw();
   }
   EndPaint(&ps);
   ValidateRect(__nullptr);
   return 0;
}

LRESULT CDWriteTab::OnScroll(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
   // Reacts to scroll bar changes.

   WORD wRequest = LOWORD(wParam);
   int nDelta = (0 == int(lParam)) ? 15 : int(lParam);
   SCROLLINFO scrollInfo = { sizeof(scrollInfo) };
   scrollInfo.fMask = SIF_ALL;
   int barOrientation = (nMsg == WM_VSCROLL) ? SB_VERT : SB_HORZ;
   if(GetScrollInfo(barOrientation, &scrollInfo))
   {
      // Save the position for comparison later on
      int oldPosition = scrollInfo.nPos;

      switch(wRequest)
      {
      case SB_TOP:        scrollInfo.nPos = scrollInfo.nMin;      break;
      case SB_BOTTOM:     scrollInfo.nPos = scrollInfo.nMax;      break;
      case SB_LINEUP:     scrollInfo.nPos -= nDelta;                   break;
      case SB_LINEDOWN:   scrollInfo.nPos += nDelta;                   break;
      case SB_PAGEUP:     scrollInfo.nPos -= scrollInfo.nPage;     break;
      case SB_PAGEDOWN:   scrollInfo.nPos += scrollInfo.nPage;     break;
      case SB_THUMBTRACK: scrollInfo.nPos = scrollInfo.nTrackPos; break;
      default:
         break;
      }

      if(scrollInfo.nPos < 0)
      {
         scrollInfo.nPos = 0;
      }
      if(scrollInfo.nPos > scrollInfo.nMax - signed(scrollInfo.nPage))
      {
         scrollInfo.nPos = scrollInfo.nMax - scrollInfo.nPage;
      }

      scrollInfo.fMask = SIF_POS;
      SetScrollInfo(barOrientation, &scrollInfo, TRUE);

      // If the position has changed, scroll the window 
      if(scrollInfo.nPos != oldPosition)
      {
         // Need the view matrix in case the editor is flipped/mirrored/rotated.
         D2D1::Matrix3x2F pageTransform;
         GetInverseViewMatrix(&DX::Cast(pageTransform));
         float inversePos = float(scrollInfo.nMax - scrollInfo.nPage - scrollInfo.nPos);

         D2D1_POINT_2F scaledSize = { pageTransform._11 + pageTransform._21,
            pageTransform._12 + pageTransform._22 };

         // Adjust the correct origin.
         if((barOrientation == SB_VERT) ^ DX::IsLandscapeAngle(m_Angle))
         {
            m_OriginY = float(scaledSize.y >= 0 ? scrollInfo.nPos : inversePos);
         }
         else
         {
            m_OriginX = float(scaledSize.x >= 0 ? scrollInfo.nPos : inversePos);
         }

         ConstrainViewOrigin();
         //PostRedraw();
      }
   }
   return 0;
}

LRESULT CDWriteTab::OnSetFocus(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
   D2D1_RECT_F caretRect;
   GetCaretRect(caretRect);
   UpdateSystemCaret(caretRect);
   return 0;
}

LRESULT CDWriteTab::OnSetInlineImage(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
   CComPtr<IFileOpenDialog> CComPtrIFileOpenDialog;
   // CoCreate the dialog object.
   HRESULT hr = CComPtrIFileOpenDialog.CoCreateInstance(CLSID_FileOpenDialog, __nullptr, CLSCTX_INPROC_SERVER);
   if(SUCCEEDED(hr))
   {
      DWORD dwOptions;
      hr = CComPtrIFileOpenDialog->GetOptions(&dwOptions);
      if(SUCCEEDED(hr))
      {
         hr = CComPtrIFileOpenDialog->SetOptions(dwOptions | FOS_ALLOWMULTISELECT);
      }
      if(SUCCEEDED(hr))
      {
         hr = CComPtrIFileOpenDialog->Show(NULL);
         if(SUCCEEDED(hr))
         {
            CComPtr<IShellItemArray> CComPtrIShellItemArray;
            hr = CComPtrIFileOpenDialog->GetResults(&CComPtrIShellItemArray);
            if(SUCCEEDED(hr))
            {
               DWORD cResults;
               hr = CComPtrIShellItemArray->GetCount(&cResults);
               if(SUCCEEDED(hr))
               {
                  for(DWORD i = 0; i < cResults; ++i)
                  {
                     IShellItem* pItem;
                     hr = CComPtrIShellItemArray->GetItemAt(i, &pItem);
                     if(SUCCEEDED(hr))
                     {
                        LPWSTR pwszPath;
                        hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pwszPath);
                        if(SUCCEEDED(hr))
                        {
                           hr = OpenImageFile(pwszPath);
                           ::CoTaskMemFree(pwszPath);
                        }
                     }
                  }
               }
            }
         }
      }
   }
   return 0;
}

LRESULT CDWriteTab::OnSize(UINT /*nMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
   D2D1_SIZE_U uSize;
   uSize.width = LOWORD(lParam);
   uSize.height = HIWORD(lParam);
   if(!(D2D1::SizeU() == uSize))
   {
      m_rcClientF.right = float(uSize.width);
      m_rcClientF.bottom = float(uSize.height);
      if(__nullptr != m_CComPtrRenderTarget)
      {
         m_CComPtrRenderTarget->Resize(uSize.width, uSize.height);
      }
   }

   RefreshView();
   return 0;
}

