

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


 /* File created by MIDL compiler version 8.00.0603 */
/* at Sun Jun 08 15:59:03 2014
 */
/* Compiler settings for hpgt33.IDL:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 8.00.0603 
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

MIDL_DEFINE_GUID(IID, LIBID_HP3300Lib,0x57819843,0x69D2,0x46DA,0x87,0xE6,0x28,0x70,0x21,0xF5,0x47,0x7C);


MIDL_DEFINE_GUID(IID, IID_IScanner,0x26955994,0x8038,0x444B,0x82,0xFD,0x79,0x4A,0x41,0x36,0x7D,0xD7);


MIDL_DEFINE_GUID(IID, IID_ILog,0xEB0D7369,0x5CB0,0x437E,0xB6,0x1B,0x1B,0x6A,0x4D,0x85,0x38,0xC0);


MIDL_DEFINE_GUID(IID, IID_IButton,0xB59EE25B,0x8A4A,0x4798,0x8B,0xB4,0xAE,0x7A,0xD3,0x3F,0xBD,0x4F);


MIDL_DEFINE_GUID(CLSID, CLSID_HP3300,0xF20FB4D8,0x0C85,0x454B,0xAA,0x3F,0x4F,0x0E,0x80,0x3A,0x0E,0x5D);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



