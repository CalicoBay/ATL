

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


 /* File created by MIDL compiler version 8.00.0603 */
/* at Mon Dec 12 14:43:11 2016
 */
/* Compiler settings for Polygon.idl:
    Oicf, W1, Zp8, env=Win64 (32b run), target_arch=AMD64 8.00.0603 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


#ifdef __cplusplus
extern "C"{
#endif 


#include <rpc.h>
#include <rpcndr.h>

#ifdef _MIDL_USE_GUIDDEF_

#ifndef INITGUID
#define INITGUID
#include <guiddef.h>
#undef INITGUID
#else
#include <guiddef.h>
#endif

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        DEFINE_GUID(name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8)

#else // !_MIDL_USE_GUIDDEF_

#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        const type name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}

#endif !_MIDL_USE_GUIDDEF_

MIDL_DEFINE_GUID(IID, IID_IPolyCtl,0xDB5FE57E,0xC11B,0x4D1B,0x94,0x37,0xE5,0xE9,0x91,0x0A,0x1D,0xBC);


MIDL_DEFINE_GUID(IID, LIBID_PolygonLib,0x6A1A47C2,0x6E64,0x4429,0x98,0x0A,0x2A,0x99,0x87,0xCE,0x66,0x43);


MIDL_DEFINE_GUID(IID, DIID__IPolyCtlEvents,0xB06174F8,0xF020,0x4F54,0x88,0xAF,0xCC,0xD9,0x19,0x39,0xD7,0xB9);


MIDL_DEFINE_GUID(CLSID, CLSID_PolyCtl,0x62F10F30,0xE30A,0x4C4C,0x9F,0x1E,0x97,0xD7,0x44,0xA5,0xE2,0x05);


MIDL_DEFINE_GUID(CLSID, CLSID_PolyProp,0xBDEBD4AE,0x68F5,0x4A16,0xAF,0xA5,0x95,0x4C,0x81,0x1D,0x6B,0x69);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



