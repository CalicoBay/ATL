// pch.cpp : source file that includes just the standard includes
// ATLDirectX1.pch will be the pre-compiled header
// pch.obj will contain the pre-compiled type information

#include "pch.h"
namespace
{
   HRESULT LoadAndLockResource(LPCWSTR wszResourceName, LPCWSTR wszResourceType, OUT UINT8** ppFileData, OUT PDWORD pdwFileSize)
   {
      HRESULT hr = E_FAIL;
      HRSRC hResource = __nullptr;
      HGLOBAL hResourceData = __nullptr;
      *ppFileData = __nullptr;
      *pdwFileSize = 0;

      // Locate the resource handle in our DLL.
      hResource = FindResourceW(GetModuleHINSTANCE(), wszResourceName, wszResourceType);
      if(__nullptr != hResource)
      {
         // Load the resource.
         hResourceData = LoadResource(GetModuleHINSTANCE(), hResource);
         if(__nullptr != hResourceData)
         {
            // Lock it to get a system memory pointer.
            *ppFileData = (BYTE*)LockResource(hResourceData);
            if(__nullptr != *ppFileData)
            {
               // Calculate the size.
               *pdwFileSize = SizeofResource(GetModuleHINSTANCE(), hResource);
               if(0 < *pdwFileSize)
               {
                  hr = S_OK;
               }
            }
         }
      }
      return hr;
   }
}

namespace DX
{
	using namespace Concurrency;
	using namespace Windows::Storage;

	void ThrowIfFailed(HRESULT hr)
	{
		if(FAILED(hr))
		{
			// Set a breakpoint on this line to catch Win32 API errors.
			throw Platform::Exception::CreateException(hr);
		}
	}

	void ThrowIfFALSE(BOOL bResult)
	{
		if(FALSE == bResult)
		{
			// Set a breakpoint on this line to catch Win32 API errors.
			throw Platform::Exception::CreateException(::GetLastError(), L"Windows API Error!");
		}
	}

    // Disassemble at this->_M_disassembleMe to get to the source location right after either the creation of the task (constructor
    // or then method) that encountered this exception, or the set_exception call for a task_completion_event.
	// Function that reads from a binary file in the current folder asynchronously.
	Concurrency::task<Platform::Array<byte>^> CreateReadDataAsync(Platform::String^ filename)
	{
		return create_task([filename]()
		{
			//WCHAR wszDir[MAX_PATH] = {};
			//ThrowIfFALSE(GetCurrentDirectory(MAX_PATH, wszDir));
			//std::wstring wsPath(wszDir);
			//wsPath.push_back(L'\\');
			//The above is fatal, even in a Win32 environment
			std::wstring wsPath(L"C:\\Users\\Public\\Documents");//Works with a terminating slash or not
			Platform::String^ psPath = ref new Platform::String(wsPath.c_str());
			return StorageFolder::GetFolderFromPathAsync(psPath);
		}).then([filename] (StorageFolder^ currentFolder)
		{
			return currentFolder->GetFileAsync(filename);
		}).then([] (StorageFile^ file) 
		{
			return FileIO::ReadBufferAsync(file);
		}).then([] (Streams::IBuffer^ fileBuffer) -> Platform::Array<byte>^ 
		{
			auto fileData = ref new Platform::Array<byte>(fileBuffer->Length);
			Streams::DataReader::FromBuffer(fileBuffer)->ReadBytes(fileData);
			return fileData;
		});
	}

	Platform::Array<byte>^ ReadDataAsync(Platform::String^ filename)
	{
		try
		{
			return(create_task([filename]()
			{
				//WCHAR wszDir[MAX_PATH] = {};
				//ThrowIfFALSE(GetCurrentDirectory(MAX_PATH, wszDir));
				//std::wstring wsPath(wszDir);
				//wsPath.push_back(L'\\');
				//The above is fatal, even in a Win32 environment
				//////////////////////Below works////////////////////////////////////////////////////////////
				//std::wstring wsPath(L"C:\\Users\\Public\\Documents");//Works with a terminating slash or not
				//Platform::String^ psPath = ref new Platform::String(wsPath.c_str());
				///////////////////////////////////Above works but we might as well do this/////////////////
				return KnownFolders::DocumentsLibrary;
			}).then([filename] (StorageFolder^ currentFolder)
			{
				return currentFolder->GetFileAsync(filename);
			}).then([] (StorageFile^ file) 
			{
				return FileIO::ReadBufferAsync(file);
			}).then([] (Streams::IBuffer^ fileBuffer) -> Platform::Array<byte>^ 
			{
				auto fileData = ref new Platform::Array<byte>(fileBuffer->Length);
				Streams::DataReader::FromBuffer(fileBuffer)->ReadBytes(fileData);
				return fileData;
			}).get());
		}
		catch(...)
		{
			Platform::Array<byte>^ nullData = ref new Platform::Array<byte>(0);
			return nullData;
		}
	}

	Windows::Storage::StorageFolder^ ReturnCurrentFolderAsync()
	{
		task<Windows::Storage::StorageFolder^> t([]()
		{
			CString sPath;
			ThrowIfFALSE(GetCurrentDirectory(0x200, sPath.GetBufferSetLength(0x200)));
			Platform::String^ psPath = ref new Platform::String(sPath);
			return Windows::Storage::StorageFolder::GetFolderFromPathAsync(psPath);
			//cancel_current_task();
		});

		Windows::Storage::StorageFolder^ returnFolder;
		try
		{
			t.wait();
		}
		catch(...)
		{
			returnFolder = KnownFolders::DocumentsLibrary;
			return returnFolder;
		}
		returnFolder = t.get();
		return returnFolder;
	}

   void ComputeInverseMatrix(
      DWRITE_MATRIX const& matrix,
      OUT DWRITE_MATRIX& result
      )
   {
      // Used for hit-testing, mouse scrolling, panning, and scroll bar sizing.

      float invdet = 1.f / GetDeterminant(matrix);
      result.m11 = matrix.m22 * invdet;
      result.m12 = -matrix.m12 * invdet;
      result.m21 = -matrix.m21 * invdet;
      result.m22 = matrix.m11 * invdet;
      result.dx = (matrix.m21 * matrix.dy - matrix.dx  * matrix.m22) * invdet;
      result.dy = (matrix.dx  * matrix.m12 - matrix.m11 * matrix.dy)  * invdet;
   }

   D2D1_POINT_2F GetPageSize(IDWriteTextLayout* textLayout)
   {
      // Use the layout metrics to determine how large the page is, taking
      // the maximum of the content size and layout's maximal dimensions.

      DWRITE_TEXT_METRICS textMetrics;
      textLayout->GetMetrics(&textMetrics);

      float width = max(textMetrics.layoutWidth, textMetrics.left + textMetrics.width);
      float height = max(textMetrics.layoutHeight, textMetrics.height);

      D2D1_POINT_2F pageSize = {width, height};
      return pageSize;
   }

   bool IsLandscapeAngle(float angle)
   {
      // Returns true if the angle is rotated 90 degrees clockwise
      // or anticlockwise (or any multiple of that).
      return fmod(abs(angle) + 45.0f, 180.0f) >= 90.0f;
   }

   HRESULT STDMETHODCALLTYPE LoadImageFromResource(LPCWSTR wszResourceName, LPCWSTR wszResourceType,
      IWICImagingFactory* pIWICImagingFactory, IWICBitmapSource** ppIWICBitmapSource)
   {
      HRESULT hr;
      DWORD dwFileSize;
      PUINT8 pFileData; // [dwFileSize]
      CComPtr<IWICStream> CComPtrIWICStream;
      CComPtr<IWICBitmapDecoder> CComPtrIWICBitmapDecoder;
      CComPtr<IWICBitmapFrameDecode> CComPtrIWICBitmapFrameDecode;
      CComPtr<IWICFormatConverter> CComPtrIWICFormatConverter;
      CComPtr<IWICImagingFactory> CComPtrIWICImagingFactory(pIWICImagingFactory);

      hr = LoadAndLockResource(wszResourceName, wszResourceType, &pFileData, &dwFileSize);
      // Create a WIC stream to map onto the memory.
      if(SUCCEEDED(hr))
      {
         hr = CComPtrIWICImagingFactory->CreateStream(&CComPtrIWICStream);
         if(SUCCEEDED(hr))
         {// Initialize the stream with the pointer and size
            hr = CComPtrIWICStream->InitializeFromMemory(pFileData, dwFileSize);
            if(SUCCEEDED(hr))
            {// Create a decoder for the stream
               hr = CComPtrIWICImagingFactory->CreateDecoderFromStream(CComPtrIWICStream, __nullptr,
                  WICDecodeMetadataCacheOnLoad, &CComPtrIWICBitmapDecoder);
               if(SUCCEEDED(hr))
               {// Create the initial frame
                  hr = CComPtrIWICBitmapDecoder->GetFrame(0, &CComPtrIWICBitmapFrameDecode);
               }
            }
         }
         if(SUCCEEDED(hr))
         {// Convert format to 32bppPBGRA - which D2D expects
            hr = CComPtrIWICImagingFactory->CreateFormatConverter(&CComPtrIWICFormatConverter);
            if(SUCCEEDED(hr))
            {
               hr = CComPtrIWICFormatConverter->Initialize
                  (
                  CComPtrIWICBitmapFrameDecode, GUID_WICPixelFormat32bppPBGRA,
                  WICBitmapDitherTypeNone, __nullptr, 0.f, WICBitmapPaletteTypeMedianCut
                  );
               if(SUCCEEDED(hr))
               {
                  *ppIWICBitmapSource = CComPtrIWICFormatConverter.Detach();
               }
            }
         }
      }
      return hr;
   }

   HRESULT STDMETHODCALLTYPE LoadImageFromFile(LPCWSTR wszFileName, IWICImagingFactory* pIWICImagingFactory, IWICBitmapSource** ppIWICBitmapSource)
   {
      HRESULT hr;
      CComPtr<IWICBitmapDecoder> CComPtrIWICBitmapDecoder;
      CComPtr<IWICBitmapFrameDecode> CComPtrIWICBitmapFrameDecode;
      CComPtr<IWICFormatConverter> CComPtrIWICFormatConverter;
      CComPtr<IWICImagingFactory> CComPtrIWICImagingFactory(pIWICImagingFactory);

      hr = CComPtrIWICImagingFactory->CreateDecoderFromFilename(wszFileName, __nullptr, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &CComPtrIWICBitmapDecoder);
      if(SUCCEEDED(hr))
      {// Create the initial frame
         hr = CComPtrIWICBitmapDecoder->GetFrame(0, &CComPtrIWICBitmapFrameDecode);
         if(SUCCEEDED(hr))
         {// Convert format to 32bppPBGRA - which D2D expects
            hr = CComPtrIWICImagingFactory->CreateFormatConverter(&CComPtrIWICFormatConverter);
            if(SUCCEEDED(hr))
            {
               hr = CComPtrIWICFormatConverter->Initialize(CComPtrIWICBitmapFrameDecode, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, __nullptr, 0.f, WICBitmapPaletteTypeMedianCut);
               if(SUCCEEDED(hr))
               {
                  *ppIWICBitmapSource = CComPtrIWICFormatConverter.Detach();
               }
            }
         }
      }
      return hr;
   }

   HRESULT STDMETHODCALLTYPE LoadShaderFromResource(LPCWSTR wszResourceName, PUINT8& pBuffer, size_t& uiCount)
   {
      DWORD dwFileSize;
      HRESULT hr = LoadAndLockResource(wszResourceName, RT_RCDATA, &pBuffer, &dwFileSize);
      uiCount = 0;
      if(SUCCEEDED(hr))
      {
         uiCount = dwFileSize;
      }
      return hr;
   }
}
