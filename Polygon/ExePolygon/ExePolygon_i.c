

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


 /* File created by MIDL compiler version 8.00.0603 */
/* at Mon Dec 12 14:43:11 2016
 */
/* Compiler settings for ExePolygon.idl:
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

MIDL_DEFINE_GUID(IID, IID_IMainWnd,0x9156F305,0xB0AB,0x40B2,0x96,0xC3,0x98,0x5F,0x2A,0x67,0x0B,0x61);


MIDL_DEFINE_GUID(IID, LIBID_ExePolygonLib,0x75E97DF7,0xC654,0x4C33,0x9C,0x4E,0x06,0x32,0xBC,0x2C,0xFD,0xC0);


MIDL_DEFINE_GUID(IID, DIID__IMainWndEvents,0x6D991963,0xAC6E,0x41D5,0x94,0xCF,0x27,0x3E,0x94,0x37,0x9A,0x33);


MIDL_DEFINE_GUID(CLSID, CLSID_MainWnd,0xE700F48F,0x0CF1,0x4C1A,0xB5,0x31,0x2E,0x99,0x67,0xD7,0x5B,0xBD);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



