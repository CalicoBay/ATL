// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved
//
// Contents:    Drawing effect that holds a single color.
// From the PadWrite example
//----------------------------------------------------------------------------
// May 27th, 2014 This code was changed to work with ATL without the use of MIDL
// DrawingEffects.h
#pragma once
extern "C" const CLSID CLSID_DrawingEffect;
typedef interface IDrawingEffect IDrawingEffect;
MIDL_INTERFACE("3FFDF222-6A6A-4C52-BC6C-38918C191EE9")IDrawingEffect : IUnknown
{
   virtual HRESULT STDMETHODCALLTYPE CreateEffect(UINT32 color) = 0;
   virtual HRESULT STDMETHODCALLTYPE GetColor(UINT32& color) = 0;
   virtual HRESULT STDMETHODCALLTYPE GetColorF(D2D1::ColorF& colorF) = 0;
   virtual HRESULT STDMETHODCALLTYPE GetCOLORREF(COLORREF& colorRef) = 0;
};
//<Guid("3FFDF222-6A6A-4C52-BC6C-38918C191EE9")>
const IID IID_IDrawingEffect = {0x3FFDF222, 0x6A6A, 0x4C52, 0xBC, 0x6C, 0x38, 0x91, 0x8C, 0x19, 0x1E, 0xE9};

class  DrawingEffect  :
   public CComObjectRoot,
   public CComCoClass<DrawingEffect, &CLSID_DrawingEffect>,
   public IDrawingEffect
{
public:
   DrawingEffect(UINT32 color = 0UL)
      : m_Color(color)
   {

   }

   DECLARE_REGISTRY_RESOURCEID(IDR_EFFECTS)
   BEGIN_COM_MAP(DrawingEffect)
      COM_INTERFACE_ENTRY(IDrawingEffect)
   END_COM_MAP()
   STDMETHOD(GetColor)(UINT32& color)
   {
      // Returns the BGRA value for D2D.
      color = m_Color;
      return S_OK;
   }

   STDMETHOD(GetColorF)(D2D1::ColorF& colorF)
   {
      float alpha = (m_Color >> 24) / 255.f;
      colorF = D2D1::ColorF(m_Color, alpha);
      return S_OK;
   }

   STDMETHOD(GetCOLORREF)(COLORREF& colorRef)
   {
      // Returns color as COLORREF.
      colorRef = GetColorRef(m_Color);
      return S_OK;
   }

   static inline COLORREF GetColorRef(UINT32 bgra) throw()
   {
      // Swaps color order (bgra <-> rgba) from D2D/GDI+'s to a COLORREF.
      // This also leaves the top byte 0, since alpha is ignored anyway.
      return RGB(GetBValue(bgra), GetGValue(bgra), GetRValue(bgra));
   }

   static inline COLORREF GetBgra(COLORREF rgb) throw()
   {
      // Swaps color order (bgra <-> rgba) from COLORREF to D2D/GDI+'s.
      // Sets alpha to full opacity.
      return RGB(GetBValue(rgb), GetGValue(rgb), GetRValue(rgb)) | 0xFF000000;
   }
   STDMETHOD (CreateEffect)(UINT32 color)
   {
      m_Color = color;
      return S_OK;
   }
protected:
   // The color is stored as BGRA, with blue in the lowest byte,
   // then green, red, alpha; which is what D2D, GDI+, and GDI DIBs use.
   // GDI's COLORREF stores red as the lowest byte.
   UINT32 m_Color;
};

OBJECT_ENTRY_AUTO(CLSID_DrawingEffect, DrawingEffect)