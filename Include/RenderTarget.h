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
// RenderTarget.h
#pragma once
interface IDrawingEffect;
extern "C" const CLSID CLSID_RenderTarget;
typedef interface IRenderTarget IRenderTarget;
MIDL_INTERFACE("56A6701D-2A37-4D1C-BB9C-3F69BAC14FCB")IRenderTarget : IDWriteTextRenderer
{
   virtual HRESULT STDMETHODCALLTYPE CreateTarget(ID2D1Factory1* pID2D1Factory1, IDWriteFactory1* pID2D1WriteFactory1, HWND hWnd) = 0;
   virtual HRESULT STDMETHODCALLTYPE BeginDraw() = 0;
   virtual HRESULT STDMETHODCALLTYPE EndDraw() = 0;
   virtual HRESULT STDMETHODCALLTYPE Clear(UINT32 color) = 0;
   virtual HRESULT STDMETHODCALLTYPE Resize(UINT width, UINT height) = 0;
   virtual HRESULT STDMETHODCALLTYPE UpdateMonitor() = 0;
   virtual HRESULT STDMETHODCALLTYPE SetTransform(D2D1_MATRIX_3X2_F const& mxTransform) = 0;
   virtual HRESULT STDMETHODCALLTYPE GetTransform(D2D1_MATRIX_3X2_F& mxTransform) = 0;
   virtual HRESULT STDMETHODCALLTYPE SetAntialiasing(bool bIsEnabled) = 0;
   virtual HRESULT STDMETHODCALLTYPE DrawTextLayout(IDWriteTextLayout* pIDWriteTextLayout, D2D1_RECT_F& rectF) = 0;
   virtual HRESULT STDMETHODCALLTYPE DrawImage
      (IWICBitmapSource* pIWICBitmapSource, const D2D1_RECT_F& sourceRectF, const D2D1_RECT_F& destRectF) = 0;
   virtual HRESULT STDMETHODCALLTYPE FillRectangle(const D2D1_RECT_F& destRectF, IDrawingEffect* pIDrawingEffect) = 0;
   virtual HRESULT STDMETHODCALLTYPE RemoveInlineObject(IDWriteInlineObject* pIDWriteInlineObject) = 0;
};
// {56A6701D-2A37-4D1C-BB9C-3F69BAC14FCB}
const IID IID_IRenderTarget = {0x56a6701d, 0x2a37, 0x4d1c, 0xbb, 0x9c, 0x3f, 0x69, 0xba, 0xc1, 0x4f, 0xcb};

class RenderTarget :
   public CComObjectRoot,
   public CComCoClass<RenderTarget, &CLSID_RenderTarget>,
   public IRenderTarget
{
public:
   DECLARE_REGISTRY_RESOURCEID(IDR_RENDERTARGET)
   BEGIN_COM_MAP(RenderTarget)
      COM_INTERFACE_ENTRY(IRenderTarget)
   END_COM_MAP()
   RenderTarget();
   virtual ~RenderTarget(){};
   virtual HRESULT STDMETHODCALLTYPE CreateTarget(ID2D1Factory1* pID2D1Factory1, IDWriteFactory1* pID2D1WriteFactory1, HWND hWnd);
   virtual HRESULT STDMETHODCALLTYPE BeginDraw();
   virtual HRESULT STDMETHODCALLTYPE EndDraw();
   virtual HRESULT STDMETHODCALLTYPE Clear(UINT32 color);
   virtual HRESULT STDMETHODCALLTYPE Resize(UINT width, UINT height);
   virtual HRESULT STDMETHODCALLTYPE UpdateMonitor();
   virtual HRESULT STDMETHODCALLTYPE SetTransform(D2D1_MATRIX_3X2_F const& mxTransform);
   virtual HRESULT STDMETHODCALLTYPE GetTransform(D2D1_MATRIX_3X2_F& mxTransform);
   virtual HRESULT STDMETHODCALLTYPE SetAntialiasing(bool bIsEnabled);
   virtual HRESULT STDMETHODCALLTYPE DrawTextLayout(IDWriteTextLayout* pIDWriteTextLayout, D2D1_RECT_F& rectF);
   virtual HRESULT STDMETHODCALLTYPE DrawImage
      (IWICBitmapSource* pIWICBitmapSource, const D2D1_RECT_F& sourceRectF, const D2D1_RECT_F& destRectF);
   virtual HRESULT STDMETHODCALLTYPE FillRectangle(const D2D1_RECT_F& destRectF, IDrawingEffect* pIDrawingEffect);
   virtual HRESULT STDMETHODCALLTYPE RemoveInlineObject(IDWriteInlineObject* IDWriteInlineObject);
   // IDWriteTextRenderer implementation
   IFACEMETHOD(DrawGlyphRun)(
      void* clientDrawingContext,
      FLOAT baselineOriginX,
      FLOAT baselineOriginY,
      DWRITE_MEASURING_MODE measuringMode,
      const DWRITE_GLYPH_RUN* glyphRun,
      const DWRITE_GLYPH_RUN_DESCRIPTION* glyphRunDescription,
      IUnknown* clientDrawingEffect
      );

   IFACEMETHOD(DrawUnderline)(
      void* clientDrawingContext,
      FLOAT baselineOriginX,
      FLOAT baselineOriginY,
      const DWRITE_UNDERLINE* underline,
      IUnknown* clientDrawingEffect
      );

   IFACEMETHOD(DrawStrikethrough)(
      void* clientDrawingContext,
      FLOAT baselineOriginX,
      FLOAT baselineOriginY,
      const DWRITE_STRIKETHROUGH* strikethrough,
      IUnknown* clientDrawingEffect
      );

   IFACEMETHOD(DrawInlineObject)(
      void* clientDrawingContext,
      FLOAT originX,
      FLOAT originY,
      IDWriteInlineObject* inlineObject,
      BOOL isSideways,
      BOOL isRightToLeft,
      IUnknown* clientDrawingEffect
      );

   IFACEMETHOD(IsPixelSnappingDisabled)(
      void* clientDrawingContext,
      OUT BOOL* isDisabled
      );

   IFACEMETHOD(GetCurrentTransform)(
      void* clientDrawingContext,
      OUT DWRITE_MATRIX* transform
      );

   IFACEMETHOD(GetPixelsPerDip)(
      void* clientDrawingContext,
      OUT FLOAT* pixelsPerDip
      );
public:
   // For cached images, to avoid needing to recreate the textures each draw call.
   struct ImageCacheEntry
   {
      ImageCacheEntry(IWICBitmapSource* initialOriginal, ID2D1Bitmap* initialConverted)
      {
         m_CComPtrOriginal = initialOriginal;
         m_CComPtrConverted = initialConverted;
      }

      ImageCacheEntry(const ImageCacheEntry& b)
      {
         m_CComPtrOriginal = b.m_CComPtrOriginal;
         m_CComPtrConverted = b.m_CComPtrConverted;
      }

      ImageCacheEntry& operator=(const ImageCacheEntry& b)
      {
         if(this != &b)
         {
            // Define assignment operator in terms of destructor and
            // placement new constructor, paying heed to self assignment.
            this->~ImageCacheEntry();
            new(this) ImageCacheEntry(b);
         }
         return *this;
      }

      ~ImageCacheEntry()
      {
         m_CComPtrOriginal.Release();
         m_CComPtrConverted.Release();
      }

      CComPtr<IWICBitmapSource> m_CComPtrOriginal;
      CComPtr<ID2D1Bitmap> m_CComPtrConverted;
   };
protected:
   // This context is not persisted, only existing on the stack as it
   // is passed down through. This is mainly needed to handle cases
   // where where no drawing effect is set, like those of an inline
   // object or trimming sign.
   struct Context
   {
      Context(RenderTarget* initialTarget, IUnknown* initialDrawingEffect)
         : target(initialTarget),
         drawingEffect(initialDrawingEffect)
      { }

      // short lived weak pointers
      RenderTarget* target;
      IUnknown* drawingEffect;
   };

   IUnknown* GetDrawingEffect(void* clientDrawingContext, IUnknown* drawingEffect)
   {
      // Callbacks use this to use a drawing effect from the client context
      // if none was passed into the callback.
      if(__nullptr != drawingEffect)
      {
         return drawingEffect;
      }

      return (reinterpret_cast<Context*>(clientDrawingContext))->drawingEffect;
   }
   ID2D1Bitmap* GetCachedImage(IWICBitmapSource* image);
   ID2D1Brush*  GetCachedBrush(IDrawingEffect* pDrawingEffect);

protected:
   CComPtr<IDWriteFactory1> m_CComPtrIDWriteFactory1;
   CComPtr<ID2D1Factory1> m_CComPtrID2D1Factory1;
   CComPtr<ID2D1HwndRenderTarget> m_CComPtrID2D1HwndRenderTarget;     // D2D render target
   CComPtr<ID2D1SolidColorBrush> m_CComPtrReusableBrush;       // reusable scratch brush for current color
   CComPtr<IDrawingEffect> m_CComPtrDefaultEffect;

   std::vector<ImageCacheEntry> m_vecImageCache;

   HWND m_hWnd;
   HMONITOR m_hMonitor;
};

OBJECT_ENTRY_AUTO(CLSID_RenderTarget, RenderTarget)
//COM doesn't work with C++ inheritance
//class RenderTargetD2D : public RenderTarget
//{
//public:
//   RenderTargetD2D();
//   virtual ~RenderTargetD2D(){};
//   virtual HRESULT STDMETHODCALLTYPE CreateTarget(ID2D1Factory1* pID2D1Factory1, IDWriteFactory1* pID2D1WriteFactory1, HWND hWnd);
//   virtual HRESULT STDMETHODCALLTYPE BeginDraw();
//   virtual HRESULT STDMETHODCALLTYPE EndDraw();
//   virtual HRESULT STDMETHODCALLTYPE Clear(UINT32 color);
//   virtual HRESULT STDMETHODCALLTYPE Resize(UINT width, UINT height);
//   virtual HRESULT STDMETHODCALLTYPE UpdateMonitor();
//   virtual HRESULT STDMETHODCALLTYPE SetTransform(DWRITE_MATRIX const& mxTransform);
//   virtual HRESULT STDMETHODCALLTYPE GetTransform(DWRITE_MATRIX& mxTransform);
//   virtual HRESULT STDMETHODCALLTYPE SetAntialiasing(bool bIsEnabled);
//   virtual HRESULT STDMETHODCALLTYPE DrawTextLayout(IDWriteTextLayout* pIDWriteTextLayout, D2D1_RECT_F& rectF);
//   virtual HRESULT STDMETHODCALLTYPE DrawImage
//      (IWICBitmapSource* pIWICBitmapSource, const D2D1_RECT_F& sourceRectF, const D2D1_RECT_F& destRectF);
//   virtual HRESULT STDMETHODCALLTYPE FillRectangle(const D2D1_RECT_F& destRectF, IDrawingEffect* pIDrawingEffect);
//   
//   // IDWriteTextRenderer implementation
//   IFACEMETHOD(DrawGlyphRun)(
//      void* clientDrawingContext,
//      FLOAT baselineOriginX,
//      FLOAT baselineOriginY,
//      DWRITE_MEASURING_MODE measuringMode,
//      const DWRITE_GLYPH_RUN* glyphRun,
//      const DWRITE_GLYPH_RUN_DESCRIPTION* glyphRunDescription,
//      IUnknown* clientDrawingEffect
//      );
//
//   IFACEMETHOD(DrawUnderline)(
//      void* clientDrawingContext,
//      FLOAT baselineOriginX,
//      FLOAT baselineOriginY,
//      const DWRITE_UNDERLINE* underline,
//      IUnknown* clientDrawingEffect
//      );
//
//   IFACEMETHOD(DrawStrikethrough)(
//      void* clientDrawingContext,
//      FLOAT baselineOriginX,
//      FLOAT baselineOriginY,
//      const DWRITE_STRIKETHROUGH* strikethrough,
//      IUnknown* clientDrawingEffect
//      );
//
//   IFACEMETHOD(DrawInlineObject)(
//      void* clientDrawingContext,
//      FLOAT originX,
//      FLOAT originY,
//      IDWriteInlineObject* inlineObject,
//      BOOL isSideways,
//      BOOL isRightToLeft,
//      IUnknown* clientDrawingEffect
//      );
//
//   IFACEMETHOD(IsPixelSnappingDisabled)(
//      void* clientDrawingContext,
//      OUT BOOL* isDisabled
//      );
//
//   IFACEMETHOD(GetCurrentTransform)(
//      void* clientDrawingContext,
//      OUT DWRITE_MATRIX* transform
//      );
//
//   IFACEMETHOD(GetPixelsPerDip)(
//      void* clientDrawingContext,
//      OUT FLOAT* pixelsPerDip
//      );
//public:
//   // For cached images, to avoid needing to recreate the textures each draw call.
//   struct ImageCacheEntry
//   {
//      ImageCacheEntry(IWICBitmapSource* initialOriginal, ID2D1Bitmap* initialConverted)
//         : original(SafeAcquire(initialOriginal)),
//         converted(SafeAcquire(initialConverted))
//      { }
//
//      ImageCacheEntry(const ImageCacheEntry& b)
//      {
//         original = SafeAcquire(b.original);
//         converted = SafeAcquire(b.converted);
//      }
//
//      ImageCacheEntry& operator=(const ImageCacheEntry& b)
//      {
//         if(this != &b)
//         {
//            // Define assignment operator in terms of destructor and
//            // placement new constructor, paying heed to self assignment.
//            this->~ImageCacheEntry();
//            new(this) ImageCacheEntry(b);
//         }
//         return *this;
//      }
//
//      ~ImageCacheEntry()
//      {
//         SafeRelease(&original);
//         SafeRelease(&converted);
//      }
//
//      IWICBitmapSource* original;
//      ID2D1Bitmap* converted;
//   };
//protected:
//   HRESULT CreateTarget();
//   ID2D1Bitmap* GetCachedImage(IWICBitmapSource* pIWICBitmapSource);
//   ID2D1Brush*  GetCachedBrush(IDrawingEffect* pIDrawingEffect);
//
//protected:
//   CComPtr<IDWriteFactory1> m_CComPtrIDWriteFactory1;
//   CComPtr<ID2D1Factory1> m_CComPtrID2D1Factory1;
//   CComPtr<ID2D1HwndRenderTarget> m_CComPtrID2D1HwndRenderTarget;     // D2D render target
//   CComPtr<ID2D1SolidColorBrush> m_CComPtrReusableBrush;       // reusable scratch brush for current color
//
//   std::vector<ImageCacheEntry> vecImageCache;
//
//   HWND m_hWnd;
//   HMONITOR m_hMonitor;
//};