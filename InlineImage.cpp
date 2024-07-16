// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved
//
// Contents:    Inline image for text layouts.
// From the PadWrite example
//----------------------------------------------------------------------------
// May 29th, 2014 This code was changed to work with ATL without the use of MIDL
// InlineImage.cpp
#include "stdafx.h"
#include "RenderTarget.h"
#include "DrawingEffects.h"
#include "InlineImage.h"

// {40A2D7ED-F52B-4792-8761-6171100BF885}
extern "C" const CLSID CLSID_InlineImage = {0x40a2d7ed, 0xf52b, 0x4792, 0x87, 0x61, 0x61, 0x71, 0x10, 0xb, 0xf8, 0x85};

   HRESULT STDMETHODCALLTYPE InlineImage::SetImage(IWICBitmapSource* pIWICBitmapSource, UINT index, UINT count)
   {
      HRESULT hr = E_INVALIDARG;
      UINT imageWidth = 0, imageHeight = 0;
      m_CComPtrIWICBitmapSource = pIWICBitmapSource;
      if(__nullptr != m_CComPtrIWICBitmapSource)
      {
         hr = m_CComPtrIWICBitmapSource->GetSize(&imageWidth, &imageHeight);
         if(SUCCEEDED(hr))
         {
            if(~0 == index)
            {// No index, use entire image
               m_rectF.left = 0;
               m_rectF.top = 0;
               m_rectF.right = (float)imageWidth;
               m_rectF.bottom = (float)imageHeight;
               hr = S_OK;
            }
            else
            {// use index //Only square images, ICONs etc., width never comes into play
               if(0 == count)
               {
                  m_rectF.bottom = (float)imageHeight;
                  m_rectF.left = index * m_rectF.bottom;
                  m_rectF.top = 0;
                  m_rectF.right = m_rectF.left + m_rectF.bottom;
                  hr = S_OK;
               }
               else
               {
                  if(0 != imageWidth % count)
                  {// Forget about it, abort
                     hr = E_INVALIDARG;
                  }
                  else
                  {
                     if(count > index)
                     {
                        float fWidth = (float)(imageWidth / count);
                        m_rectF.left = index * fWidth;
                        m_rectF.top = 0;
                        m_rectF.right = m_rectF.left + fWidth;
                        m_rectF.bottom = (float)imageHeight;
                        hr = S_OK;
                     }
                     else
                     {
                        hr = E_INVALIDARG;
                     }
                  }
               }
            }
         }
         if(SUCCEEDED(hr))
         {
            m_fBaseline = (float)imageHeight;
         }
      }
      else
      {
         hr = E_POINTER;
      }
      return hr;
   }

   HRESULT STDMETHODCALLTYPE InlineImage::Draw(PVOID pClientDrawingContext, IDWriteTextRenderer* pIDWriteTextRenderer,
      FLOAT fOriginX, FLOAT fOriginY, BOOL bIsSideways, BOOL bIsRightToLeft,
      IUnknown* pIUnkClientDrawingEffect)
   {
      HRESULT hr = S_OK;
      // Go from the text renderer interface back to the actual render target.
      CComPtr<IDWriteTextRenderer> CComPtrIDWriteTextRenderer(pIDWriteTextRenderer);
      CComPtr<IRenderTarget> CComPtrIRenderTarget;
      hr = CComPtrIDWriteTextRenderer.QueryInterface(&CComPtrIRenderTarget);
      if(SUCCEEDED(hr))
      {
         float fHeight = m_rectF.bottom - m_rectF.top;
         float fWidth = m_rectF.right - m_rectF.left;
         //if(16.f < fWidth)
         //{
         //   __debugbreak();
         //}
         D2D1_RECT_F destRectF = {fOriginX, fOriginY, fOriginX + fWidth, fOriginY + fHeight};

         hr = CComPtrIRenderTarget->DrawImage(m_CComPtrIWICBitmapSource, m_rectF, destRectF);
      }
      return hr;
   }

   HRESULT STDMETHODCALLTYPE InlineImage::GetMetrics(OUT DWRITE_INLINE_OBJECT_METRICS* pMetrics)
   {
      HRESULT hr = E_INVALIDARG;
      if(__nullptr != pMetrics)
      {
         DWRITE_INLINE_OBJECT_METRICS inlineMetrics = {};
         inlineMetrics.width = m_rectF.right - m_rectF.left;
         inlineMetrics.height = m_rectF.bottom - m_rectF.top;
         inlineMetrics.baseline = m_fBaseline;
         *pMetrics = inlineMetrics;
         hr = S_OK;
      }
      return hr;
   }


   HRESULT STDMETHODCALLTYPE InlineImage::GetOverhangMetrics(OUT DWRITE_OVERHANG_METRICS* pOverhangs)
   {
      HRESULT hr = E_INVALIDARG;
      if(__nullptr != pOverhangs)
      {
         pOverhangs->left = 0;
         pOverhangs->top = 0;
         pOverhangs->right = 0;
         pOverhangs->bottom = 0;
         hr = S_OK;
      }
      return hr;
   }


   HRESULT STDMETHODCALLTYPE InlineImage::GetBreakConditions(OUT DWRITE_BREAK_CONDITION* pBreakConditionBefore, OUT DWRITE_BREAK_CONDITION* pBreakConditionAfter)
   {
      HRESULT hr = E_INVALIDARG;
      if(__nullptr != pBreakConditionBefore && __nullptr != pBreakConditionAfter)
      {
         *pBreakConditionBefore = DWRITE_BREAK_CONDITION_NEUTRAL;
         *pBreakConditionAfter = DWRITE_BREAK_CONDITION_NEUTRAL;
         hr = S_OK;
      }
      return hr;
   }

