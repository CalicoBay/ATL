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
// InlineImage.h
#pragma once

interface IWICBitmapSource;
extern "C" const CLSID CLSID_InlineImage;
typedef interface IInlineImage IInlineImage;
MIDL_INTERFACE("C4308CF8-D90E-4BC5-A6E5-66A835BE925C")IInlineImage : IDWriteInlineObject
{
   virtual HRESULT STDMETHODCALLTYPE SetImage(IWICBitmapSource* pIWICBitmapSource, UINT index = ~0, UINT count = 0) = 0;
   virtual HRESULT STDMETHODCALLTYPE GetImage(IWICBitmapSource** ppIWICBitmapSource) = 0;
};
const IID IID_IInlineImage = {0xc4308cf8, 0xd90e, 0x4bc5, 0xa6, 0xe5, 0x66, 0xa8, 0x35, 0xbe, 0x92, 0x5c};

class InlineImage :
   public CComObjectRoot,
   public CComCoClass<InlineImage, &CLSID_InlineImage>,
   public IInlineImage
{
public:
   DECLARE_REGISTRY_RESOURCEID(IDR_INLINEIMAGE)
   BEGIN_COM_MAP(InlineImage)
      COM_INTERFACE_ENTRY(IInlineImage)
   END_COM_MAP()
   InlineImage(){ m_fBaseline = 0.f; m_rectF = D2D1::RectF(); };
   ~InlineImage(){};
   HRESULT STDMETHODCALLTYPE SetImage(IWICBitmapSource* pIWICBitmapSource, UINT index = ~0, UINT count = 0);
   HRESULT STDMETHODCALLTYPE GetImage(IWICBitmapSource** ppIWICBitmapSource){ *ppIWICBitmapSource = m_CComPtrIWICBitmapSource.p; return S_OK; }
   IFACEMETHOD(Draw)(PVOID pClientDrawingContext, IDWriteTextRenderer* pIDWriteTextRenderer,
      FLOAT fOriginX, FLOAT fOriginY, BOOL bIsSideways, BOOL bIsRightToLeft, IUnknown* pIUnkClientDrawingEffect);

   IFACEMETHOD(GetMetrics)(OUT DWRITE_INLINE_OBJECT_METRICS* pMetrics);

   IFACEMETHOD(GetOverhangMetrics)(OUT DWRITE_OVERHANG_METRICS* pOverhangs);

   IFACEMETHOD(GetBreakConditions)(OUT DWRITE_BREAK_CONDITION* pBreakConditionBefore, OUT DWRITE_BREAK_CONDITION* pBreakConditionAfter);
protected:
   CComPtr<IWICBitmapSource> m_CComPtrIWICBitmapSource;
   D2D1_RECT_F m_rectF; // coordinates in image, similar to index of HIMAGELIST
   float m_fBaseline;
};

OBJECT_ENTRY_AUTO(CLSID_InlineImage, InlineImage)