// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved
//
// Contents:    Adapter render target draws using D2D or DirectWrite.
//              This demonstrates how to implement your own render target
//              for layout drawing callbacks.
// From the PadWrite example
//----------------------------------------------------------------------------
// May 28th, 2014 This code was changed to work with ATL without the use of MIDL
// Only the D2D implementation is here, as true COM requires two separate interfaces
// RenderTarget.cpp
#include "stdafx.h"
#include "DrawingEffects.h"
#include "InlineImage.h"
#include "RenderTarget.h"

// {C949F90A-1939-423E-9D0C-013894B3AFB7}
extern "C" const CLSID CLSID_RenderTarget = {0xc949f90a, 0x1939, 0x423e, 0x9d, 0xc, 0x1, 0x38, 0x94, 0xb3, 0xaf, 0xb7};

inline bool operator== (const RenderTarget::ImageCacheEntry& entry, const IWICBitmapSource* original)
{
   return entry.m_CComPtrOriginal == original;
}


RenderTarget::RenderTarget() :
   m_hWnd(__nullptr),
   m_hMonitor(__nullptr)
   {}

ID2D1Brush*  RenderTarget::GetCachedBrush(IDrawingEffect* pIDrawingEffect)
{
   if(__nullptr == pIDrawingEffect)
   {
      if(__nullptr == m_CComPtrReusableBrush)
      {
         return __nullptr;
      }
      else
      {
         return m_CComPtrReusableBrush;
      }
   }
   D2D1::ColorF colorF(0, 0);
   pIDrawingEffect->GetColorF(colorF);
   m_CComPtrReusableBrush->SetColor(colorF);
   return m_CComPtrReusableBrush;
}

ID2D1Bitmap* RenderTarget::GetCachedImage(IWICBitmapSource* pIWICBitmapSource)
{
   ID2D1Bitmap* pID2D1Bitmap = __nullptr;
   if(__nullptr != pIWICBitmapSource)
   {
      std::vector<ImageCacheEntry>::iterator itMatch
         = std::find(m_vecImageCache.begin(), m_vecImageCache.end(), pIWICBitmapSource);
      if(m_vecImageCache.end() != itMatch)
      {
         return itMatch->m_CComPtrConverted;// Already cached
      }
      // Convert the WIC image to a ready-to-use device-dependent D2D bitmap.
      // This avoids needing to recreate a new texture every draw call, but
      // allows easy reconstruction of textures if the device changes and
      // resources need recreation (also lets callers be D2D agnostic).
      HRESULT hr = m_CComPtrID2D1HwndRenderTarget->CreateBitmapFromWicBitmap(pIWICBitmapSource, &pID2D1Bitmap);
      if(SUCCEEDED(hr))
      {
         try
         {
            m_vecImageCache.push_back(ImageCacheEntry(pIWICBitmapSource, pID2D1Bitmap));
         }
         catch(...)
         {
            // Most likely out of memory
            pID2D1Bitmap->Release();
            pID2D1Bitmap = __nullptr;
         }
         // If not out of memory..
         // Release it locally and return the pointer.
         // The bitmap is now referenced by the bitmap cache.
         if(__nullptr != pID2D1Bitmap)
         {
            pID2D1Bitmap->Release();
         }
      }
   }
   return pID2D1Bitmap;
}

HRESULT STDMETHODCALLTYPE RenderTarget::CreateTarget(ID2D1Factory1* pID2D1Factory1, IDWriteFactory1* pID2D1WriteFactory1, HWND hWnd)
{
   HRESULT hr = S_OK;
   m_CComPtrID2D1Factory1 = pID2D1Factory1;
   m_CComPtrIDWriteFactory1 = pID2D1WriteFactory1;
   m_hWnd = hWnd;
   m_CComPtrID2D1HwndRenderTarget = __nullptr;
   m_CComPtrReusableBrush = __nullptr;
   if(__nullptr == m_CComPtrID2D1Factory1.p || __nullptr == m_CComPtrIDWriteFactory1 || __nullptr == m_hWnd)
   {
      hr = E_INVALIDARG;
   }
   if(SUCCEEDED(hr))
   {
      RECT rcClient;
      ::GetClientRect(m_hWnd, &rcClient);
      D2D1_SIZE_U sizeU = D2D1::SizeU(rcClient.right, rcClient.bottom);
      hr = m_CComPtrID2D1Factory1->CreateHwndRenderTarget
         (D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(m_hWnd, sizeU), &m_CComPtrID2D1HwndRenderTarget);
   }
   if(SUCCEEDED(hr))
   {  // From the original author----
      // Any scaling will be combined into matrix transforms rather than an
      // additional DPI scaling. This simplifies the logic for rendering
      // and hit-testing. If an application does not use matrices, then
      // using the scaling factor directly is simpler.
      m_CComPtrID2D1HwndRenderTarget->SetDpi(96.f, 96.f);
      // Create a reusable scratch brush
      hr = m_CComPtrID2D1HwndRenderTarget->CreateSolidColorBrush
         (D2D1::ColorF(D2D1::ColorF::Red), &m_CComPtrReusableBrush);
      if(SUCCEEDED(hr))
      {
         hr = m_CComPtrDefaultEffect.CoCreateInstance(CLSID_DrawingEffect, __nullptr, CLSCTX_INPROC_SERVER);
         if(SUCCEEDED(hr))
         {
            hr = m_CComPtrDefaultEffect->CreateEffect(0xFF000000 | D2D1::ColorF::Red);
         }
      }
   }
   if(SUCCEEDED(hr))
   {
      UpdateMonitor();
   }
   return hr;
}

HRESULT STDMETHODCALLTYPE RenderTarget::BeginDraw()
{
   m_CComPtrID2D1HwndRenderTarget->BeginDraw();
   m_CComPtrID2D1HwndRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
   return S_OK;
}

HRESULT STDMETHODCALLTYPE RenderTarget::EndDraw()
{
   HRESULT hr = m_CComPtrID2D1HwndRenderTarget->EndDraw();
   if(D2DERR_RECREATE_TARGET == hr)
   {
      RECT rcClient;
      ::GetClientRect(m_hWnd, &rcClient);
      D2D1_SIZE_U sizeU = D2D1::SizeU(rcClient.right, rcClient.bottom);
      m_vecImageCache.clear();
      m_hMonitor = __nullptr;
      m_CComPtrID2D1HwndRenderTarget.Release();
      m_CComPtrReusableBrush.Release();
      hr = m_CComPtrID2D1Factory1->CreateHwndRenderTarget
         (D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(m_hWnd, sizeU), &m_CComPtrID2D1HwndRenderTarget);

      if(SUCCEEDED(hr))
      {  // From the original author----
         // Any scaling will be combined into matrix transforms rather than an
         // additional DPI scaling. This simplifies the logic for rendering
         // and hit-testing. If an application does not use matrices, then
         // using the scaling factor directly is simpler.
         m_CComPtrID2D1HwndRenderTarget->SetDpi(96.f, 96.f);
         hr = m_CComPtrID2D1HwndRenderTarget->CreateSolidColorBrush
            (D2D1::ColorF(D2D1::ColorF::White), &m_CComPtrReusableBrush);
      }
      if(SUCCEEDED(hr))
      {
         UpdateMonitor();
      }
   }
   return hr;
}

HRESULT STDMETHODCALLTYPE RenderTarget::Clear(UINT32 color)
{
   m_CComPtrID2D1HwndRenderTarget->Clear(D2D1::ColorF(color));
   return S_OK;
}

HRESULT STDMETHODCALLTYPE RenderTarget::Resize(UINT width, UINT height)
{
   D2D1_SIZE_U sizeU = {width, height};
   return m_CComPtrID2D1HwndRenderTarget->Resize(sizeU);
}

HRESULT STDMETHODCALLTYPE RenderTarget::UpdateMonitor()
{// Updates rendering parameters according to current monitor
   HRESULT hr = S_OK;
   HMONITOR monitor = MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST);
   if(monitor != m_hMonitor)
   {// Create based on monitor settings, rather than the defaults of
      // gamma=1.8, contrast=.5, and clearTypeLevel=.5
      IDWriteRenderingParams* pIDWriteRenderingParams = __nullptr;
      hr = m_CComPtrIDWriteFactory1->CreateMonitorRenderingParams(monitor, &pIDWriteRenderingParams);
      if(SUCCEEDED(hr))
      {
         m_CComPtrID2D1HwndRenderTarget->SetTextRenderingParams(pIDWriteRenderingParams);
         m_hMonitor = monitor;
         InvalidateRect(m_hWnd, __nullptr, FALSE);
         pIDWriteRenderingParams->Release();
      }
   }
   return hr;
}

HRESULT STDMETHODCALLTYPE RenderTarget::SetTransform(D2D1_MATRIX_3X2_F const& mxTransform)
{
   m_CComPtrID2D1HwndRenderTarget->SetTransform(mxTransform);
   return S_OK;
}
HRESULT STDMETHODCALLTYPE RenderTarget::GetTransform(D2D1_MATRIX_3X2_F& mxTransform)
{
   m_CComPtrID2D1HwndRenderTarget->GetTransform(&mxTransform);
   return S_OK;
}

HRESULT STDMETHODCALLTYPE RenderTarget::SetAntialiasing(bool bIsEnabled)
{
   m_CComPtrID2D1HwndRenderTarget->SetAntialiasMode(bIsEnabled ? D2D1_ANTIALIAS_MODE_PER_PRIMITIVE : D2D1_ANTIALIAS_MODE_ALIASED);
   return S_OK;
}

HRESULT STDMETHODCALLTYPE RenderTarget::DrawTextLayout(IDWriteTextLayout* pIDWriteTextLayout, D2D1_RECT_F& rectF)
{
   HRESULT hr = E_INVALIDARG;
   if(__nullptr != pIDWriteTextLayout)
   {
      Context context(this, m_CComPtrDefaultEffect);
      hr = pIDWriteTextLayout->Draw(&context, this, rectF.left, rectF.top);
   }
   return hr;
}

HRESULT STDMETHODCALLTYPE RenderTarget::DrawImage(IWICBitmapSource* pIWICBitmapSource, const D2D1_RECT_F& sourceRectF, const D2D1_RECT_F& destRectF)
{
   HRESULT hr = S_OK;
   // Ignore zero size source rects.
   // Draw nothing if the destination is zero size.
   if(__nullptr == &sourceRectF || sourceRectF.left >= sourceRectF.right || sourceRectF.top >= sourceRectF.bottom
         || destRectF.left >= destRectF.right || destRectF.top >= destRectF.bottom)
   {
      return hr;
   }
   ID2D1Bitmap* pID2D1Bitmap = GetCachedImage(pIWICBitmapSource);
   if(__nullptr != pID2D1Bitmap)
   {
      m_CComPtrID2D1HwndRenderTarget->DrawBitmap(pID2D1Bitmap, destRectF, 1.0,//Opacity
            D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, sourceRectF);
   }
   return hr;
}

HRESULT STDMETHODCALLTYPE RenderTarget::FillRectangle(const D2D1_RECT_F& destRectF, IDrawingEffect* pIDrawingEffect)
{
   ID2D1Brush* pID2D1Brush = GetCachedBrush(pIDrawingEffect);
   if(__nullptr != pID2D1Brush)
   {
      m_CComPtrID2D1HwndRenderTarget->FillRectangle(destRectF, pID2D1Brush);
   }
   return S_OK;
}

HRESULT STDMETHODCALLTYPE RenderTarget::RemoveInlineObject(IDWriteInlineObject* pIDWriteInlineObject)
{
   HRESULT hr = S_FALSE;
   IWICBitmapSource* pIWICBitmapSource = __nullptr;
   IInlineImage* pIInlineImage = reinterpret_cast<IInlineImage*>(pIDWriteInlineObject);
   if(__nullptr != pIInlineImage)
   {
      hr = pIInlineImage->GetImage(&pIWICBitmapSource);
      if(SUCCEEDED(hr))
      {
         std::vector<ImageCacheEntry>::iterator itMatch = std::find(m_vecImageCache.begin(), m_vecImageCache.end(), pIWICBitmapSource);
         if(m_vecImageCache.end() != itMatch)
         {
            itMatch->m_CComPtrOriginal.Release();
            itMatch->m_CComPtrConverted.Release();
            m_vecImageCache.erase(itMatch);
            //pIWICBitmapSource->Release();
            //pIInlineImage->Release();
            hr = S_OK;
         }
      }
   }
   return hr;
}

//////////////////////IDWriteTextRenderer implementation
HRESULT STDMETHODCALLTYPE RenderTarget::DrawGlyphRun(
   void* clientDrawingContext,
   FLOAT baselineOriginX,
   FLOAT baselineOriginY,
   DWRITE_MEASURING_MODE measuringMode,
   const DWRITE_GLYPH_RUN* glyphRun,
   const DWRITE_GLYPH_RUN_DESCRIPTION* glyphRunDescription,
   IUnknown* clientDrawingEffect
   )
{
   // If no drawing effect is applied to run, but a clientDrawingContext
   // is passed, use the one from that instead. This is useful for trimming
   // signs, where they don't have a color of their own.
   clientDrawingEffect = GetDrawingEffect(clientDrawingContext, clientDrawingEffect);

   // Since we use our own custom renderer and explicitly set the effect
   // on the layout, we know exactly what the parameter is and can
   // safely cast it directly.
   IDrawingEffect* effect = reinterpret_cast<IDrawingEffect*>(clientDrawingEffect);
   ID2D1Brush* brush = GetCachedBrush(effect);
   if(__nullptr == brush)
   {
      return E_FAIL;
   }
   m_CComPtrID2D1HwndRenderTarget->DrawGlyphRun(
      D2D1::Point2(baselineOriginX, baselineOriginY), glyphRun, brush, measuringMode);

   return S_OK;
}


HRESULT STDMETHODCALLTYPE RenderTarget::DrawUnderline(
   void* clientDrawingContext,
   FLOAT baselineOriginX,
   FLOAT baselineOriginY,
   const DWRITE_UNDERLINE* underline,
   IUnknown* clientDrawingEffect
   )
{
   clientDrawingEffect = GetDrawingEffect(clientDrawingContext, clientDrawingEffect);

   IDrawingEffect* effect = reinterpret_cast<IDrawingEffect*>(clientDrawingEffect);
   ID2D1Brush* brush = GetCachedBrush(effect);
   if(brush == NULL)
      return E_FAIL;

   // We will always get a strikethrough as a LTR rectangle
   // with the baseline origin snapped.
   D2D1_RECT_F rectangle =
   {
      baselineOriginX,
      baselineOriginY + underline->offset,
      baselineOriginX + underline->width,
      baselineOriginY + underline->offset + underline->thickness
   };

   // Draw this as a rectangle, rather than a line.
   m_CComPtrID2D1HwndRenderTarget->FillRectangle(&rectangle, brush);

   return S_OK;
}


HRESULT STDMETHODCALLTYPE RenderTarget::DrawStrikethrough(
   void* clientDrawingContext,
   FLOAT baselineOriginX,
   FLOAT baselineOriginY,
   const DWRITE_STRIKETHROUGH* strikethrough,
   IUnknown* clientDrawingEffect
   )
{
   clientDrawingEffect = GetDrawingEffect(clientDrawingContext, clientDrawingEffect);

   IDrawingEffect* effect = reinterpret_cast<IDrawingEffect*>(clientDrawingEffect);
   ID2D1Brush* brush = GetCachedBrush(effect);
   if(brush == NULL)
      return E_FAIL;

   // We will always get an underline as a LTR rectangle
   // with the baseline origin snapped.
   D2D1_RECT_F rectangle =
   {
      baselineOriginX,
      baselineOriginY + strikethrough->offset,
      baselineOriginX + strikethrough->width,
      baselineOriginY + strikethrough->offset + strikethrough->thickness
   };

   // Draw this as a rectangle, rather than a line.
   m_CComPtrID2D1HwndRenderTarget->FillRectangle(&rectangle, brush);

   return S_OK;
}


HRESULT STDMETHODCALLTYPE RenderTarget::DrawInlineObject(
   void* clientDrawingContext,
   FLOAT originX,
   FLOAT originY,
   IDWriteInlineObject* inlineObject,
   BOOL isSideways,
   BOOL isRightToLeft,
   IUnknown* clientDrawingEffect
   )
{
   // Inline objects inherit the drawing effect of the text
   // they are in, so we should pass it down (if none is set
   // on this range, use the drawing context's effect instead).
   Context subContext(*reinterpret_cast<RenderTarget::Context*>(clientDrawingContext));

   if(clientDrawingEffect != NULL)
      subContext.drawingEffect = clientDrawingEffect;

   inlineObject->Draw(
      &subContext,
      this,
      originX,
      originY,
      false,
      false,
      subContext.drawingEffect
      );

   return S_OK;
}


HRESULT STDMETHODCALLTYPE RenderTarget::IsPixelSnappingDisabled(
   void* clientDrawingContext,
   OUT BOOL* isDisabled
   )
{
   // Enable pixel snapping of the text baselines,
   // since we're not animating and don't want blurry text.
   *isDisabled = FALSE;
   return S_OK;
}


HRESULT STDMETHODCALLTYPE RenderTarget::GetCurrentTransform(
   void* clientDrawingContext,
   OUT DWRITE_MATRIX* transform
   )
{
   // Simply forward what the real renderer holds onto.
   m_CComPtrID2D1HwndRenderTarget->GetTransform(reinterpret_cast<D2D1_MATRIX_3X2_F*>(transform));
   return S_OK;
}


HRESULT STDMETHODCALLTYPE RenderTarget::GetPixelsPerDip(
   void* clientDrawingContext,
   OUT FLOAT* pixelsPerDip
   )
{
   // Any scaling will be combined into matrix transforms rather than an
   // additional DPI scaling. This simplifies the logic for rendering
   // and hit-testing. If an application does not use matrices, then
   // using the scaling factor directly is simpler.
   *pixelsPerDip = 1;
   return S_OK;
}


