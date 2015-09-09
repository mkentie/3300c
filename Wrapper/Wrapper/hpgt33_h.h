

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


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


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__


#ifndef __hpgt33_h_h__
#define __hpgt33_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IScanner_FWD_DEFINED__
#define __IScanner_FWD_DEFINED__
typedef interface IScanner IScanner;

#endif 	/* __IScanner_FWD_DEFINED__ */


#ifndef __ILog_FWD_DEFINED__
#define __ILog_FWD_DEFINED__
typedef interface ILog ILog;

#endif 	/* __ILog_FWD_DEFINED__ */


#ifndef __IButton_FWD_DEFINED__
#define __IButton_FWD_DEFINED__
typedef interface IButton IButton;

#endif 	/* __IButton_FWD_DEFINED__ */


#ifndef __HP3300_FWD_DEFINED__
#define __HP3300_FWD_DEFINED__

#ifdef __cplusplus
typedef class HP3300 HP3300;
#else
typedef struct HP3300 HP3300;
#endif /* __cplusplus */

#endif 	/* __HP3300_FWD_DEFINED__ */


#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __HP3300Lib_LIBRARY_DEFINED__
#define __HP3300Lib_LIBRARY_DEFINED__

/* library HP3300Lib */
/* [helpstring][version][uuid] */ 




typedef struct tagSCANNER_CAPABILITIES
    {
    unsigned char byModelNumber[ 16 ];
    unsigned short wLampWarmupTime;
    unsigned short wOpticalResolution;
    unsigned short wMaximumXResolution;
    unsigned short wMaximumYResolution;
    unsigned short wMinimumResolution;
    unsigned long dwMaximumWidth;
    unsigned long dwMinimumWidth;
    unsigned long dwMaximumHeight;
    unsigned long dwMinimumHeight;
    unsigned short wRawPixelOrder;
    unsigned short wPixelPacking;
    unsigned long dwScanModesSupported;
    unsigned long dwScanOptionsSupported;
    unsigned short wGreyChannelsSupported;
    unsigned short wPaperPath;
    unsigned short wGammaTableSize;
    unsigned char byColorBitsPerChannel[ 8 ];
    unsigned char byGreyBitsPerChannel[ 8 ];
    unsigned long dwReserved[ 11 ];
    } 	SCANNER_CAPABILITIES;

typedef struct tagSCANNER_PARAMETERS
    {
    unsigned long dwLeft;
    unsigned long dwTop;
    unsigned long dwWidth;
    unsigned long dwLength;
    unsigned long dwScanMode;
    unsigned long dwScanOptions;
    unsigned char byBitsPerChannel;
    unsigned short wXResolution;
    unsigned short wYResolution;
    unsigned short wGreyChannel;
    unsigned short wScanMethod;
    unsigned long dwReserved[ 12 ];
    } 	SCANNER_PARAMETERS;

typedef struct tagBUFFER_INFO
    {
    unsigned long dwPixelWidth;
    unsigned long dwPixelHeight;
    unsigned long dwBytesPerLine;
    unsigned long dwReserved[ 10 ];
    } 	BUFFER_INFO;


EXTERN_C const IID LIBID_HP3300Lib;

#ifndef __IScanner_INTERFACE_DEFINED__
#define __IScanner_INTERFACE_DEFINED__

/* interface IScanner */
/* [object][helpstring][uuid] */ 


EXTERN_C const IID IID_IScanner;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("26955994-8038-444B-82FD-794A41367DD7")
    IScanner : public IUnknown
    {
    public:
        virtual HRESULT __stdcall OpenScannerSession( 
            /* [in] */ LPWSTR pszDevName,
            /* [in] */ unsigned long dwDevIOType,
            /* [out] */ unsigned long *phSession,
            /* [out] */ unsigned long *pdwErrorCode) = 0;
        
        virtual HRESULT __stdcall CloseScannerSession( 
            /* [in] */ unsigned long hSession,
            /* [out] */ unsigned long *pdwErrorCode) = 0;
        
        virtual HRESULT __stdcall LockScanner( 
            /* [in] */ unsigned long hSession,
            /* [in] */ unsigned short wLockType,
            /* [in] */ unsigned long dwTimeout,
            /* [out] */ unsigned long *pdwErrorCode) = 0;
        
        virtual HRESULT __stdcall UnlockScanner( 
            /* [in] */ unsigned long hSession,
            /* [out] */ unsigned long *pdwErrorCode) = 0;
        
        virtual HRESULT __stdcall GetLampStatus( 
            /* [in] */ unsigned long hSession,
            /* [out] */ unsigned char *pbLampOn,
            /* [out] */ unsigned long *pdwErrorCode) = 0;
        
        virtual HRESULT __stdcall LampOn( 
            /* [in] */ unsigned long hSession,
            /* [out] */ unsigned long *pdwErrorCode) = 0;
        
        virtual HRESULT __stdcall LampOff( 
            /* [in] */ unsigned long hSession,
            /* [out] */ unsigned long *pdwErrorCode) = 0;
        
        virtual HRESULT __stdcall GetScannerCapabilities( 
            /* [in] */ unsigned long hSession,
            /* [out] */ SCANNER_CAPABILITIES *pScannerCapabilities,
            /* [out] */ unsigned long *pdwErrorCode) = 0;
        
        virtual HRESULT __stdcall GetBufferInfo( 
            /* [in] */ unsigned long hSession,
            /* [in] */ SCANNER_PARAMETERS *pScanParam,
            /* [out] */ BUFFER_INFO *pBufferInfo,
            /* [out] */ unsigned long *pdwdErrorCode) = 0;
        
        virtual HRESULT __stdcall InitializeScanner( 
            /* [in] */ unsigned long hSession,
            /* [out] */ unsigned long *pdwErrorCode) = 0;
        
        virtual HRESULT __stdcall ResetScanner( 
            /* [in] */ unsigned long hSession,
            /* [out] */ unsigned long *pdwErrorCode) = 0;
        
        virtual HRESULT __stdcall SetScannerParameters( 
            /* [in] */ unsigned long hSession,
            /* [in] */ SCANNER_PARAMETERS *pScanParam,
            /* [out] */ unsigned long *pdwErrorCode) = 0;
        
        virtual HRESULT __stdcall SetGammaTable( 
            /* [in] */ unsigned long hSession,
            /* [in] */ unsigned long uSz,
            /* [in] */ unsigned char *pbyRed,
            /* [in] */ unsigned char *pbyGreen,
            /* [in] */ unsigned char *pbyBlue,
            /* [out] */ unsigned long *pdwErrorCode) = 0;
        
        virtual HRESULT __stdcall StartScan( 
            /* [in] */ unsigned long hSession,
            /* [out] */ unsigned long *pdwErrorCode) = 0;
        
        virtual HRESULT __stdcall StopScan( 
            /* [in] */ unsigned long hSession,
            /* [out] */ unsigned long *pdwErrorCode) = 0;
        
        virtual HRESULT __stdcall ReadScan( 
            /* [in] */ unsigned long hSession,
            /* [out] */ unsigned char *pbyBuffer,
            /* [in] */ unsigned long dwBytes,
            /* [out] */ unsigned long *pdwErrorCode) = 0;
        
        virtual HRESULT __stdcall ReadScanEx( 
            /* [in] */ unsigned long hSession,
            /* [out] */ unsigned char *pbyBuffer,
            /* [in] */ unsigned long dwBytes,
            /* [out] */ unsigned long *pdwBytesRead,
            /* [out] */ unsigned long *pdwErrorCode) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IScannerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IScanner * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IScanner * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IScanner * This);
        
        HRESULT ( __stdcall *OpenScannerSession )( 
            IScanner * This,
            /* [in] */ LPWSTR pszDevName,
            /* [in] */ unsigned long dwDevIOType,
            /* [out] */ unsigned long *phSession,
            /* [out] */ unsigned long *pdwErrorCode);
        
        HRESULT ( __stdcall *CloseScannerSession )( 
            IScanner * This,
            /* [in] */ unsigned long hSession,
            /* [out] */ unsigned long *pdwErrorCode);
        
        HRESULT ( __stdcall *LockScanner )( 
            IScanner * This,
            /* [in] */ unsigned long hSession,
            /* [in] */ unsigned short wLockType,
            /* [in] */ unsigned long dwTimeout,
            /* [out] */ unsigned long *pdwErrorCode);
        
        HRESULT ( __stdcall *UnlockScanner )( 
            IScanner * This,
            /* [in] */ unsigned long hSession,
            /* [out] */ unsigned long *pdwErrorCode);
        
        HRESULT ( __stdcall *GetLampStatus )( 
            IScanner * This,
            /* [in] */ unsigned long hSession,
            /* [out] */ unsigned char *pbLampOn,
            /* [out] */ unsigned long *pdwErrorCode);
        
        HRESULT ( __stdcall *LampOn )( 
            IScanner * This,
            /* [in] */ unsigned long hSession,
            /* [out] */ unsigned long *pdwErrorCode);
        
        HRESULT ( __stdcall *LampOff )( 
            IScanner * This,
            /* [in] */ unsigned long hSession,
            /* [out] */ unsigned long *pdwErrorCode);
        
        HRESULT ( __stdcall *GetScannerCapabilities )( 
            IScanner * This,
            /* [in] */ unsigned long hSession,
            /* [out] */ SCANNER_CAPABILITIES *pScannerCapabilities,
            /* [out] */ unsigned long *pdwErrorCode);
        
        HRESULT ( __stdcall *GetBufferInfo )( 
            IScanner * This,
            /* [in] */ unsigned long hSession,
            /* [in] */ SCANNER_PARAMETERS *pScanParam,
            /* [out] */ BUFFER_INFO *pBufferInfo,
            /* [out] */ unsigned long *pdwdErrorCode);
        
        HRESULT ( __stdcall *InitializeScanner )( 
            IScanner * This,
            /* [in] */ unsigned long hSession,
            /* [out] */ unsigned long *pdwErrorCode);
        
        HRESULT ( __stdcall *ResetScanner )( 
            IScanner * This,
            /* [in] */ unsigned long hSession,
            /* [out] */ unsigned long *pdwErrorCode);
        
        HRESULT ( __stdcall *SetScannerParameters )( 
            IScanner * This,
            /* [in] */ unsigned long hSession,
            /* [in] */ SCANNER_PARAMETERS *pScanParam,
            /* [out] */ unsigned long *pdwErrorCode);
        
        HRESULT ( __stdcall *SetGammaTable )( 
            IScanner * This,
            /* [in] */ unsigned long hSession,
            /* [in] */ unsigned long uSz,
            /* [in] */ unsigned char *pbyRed,
            /* [in] */ unsigned char *pbyGreen,
            /* [in] */ unsigned char *pbyBlue,
            /* [out] */ unsigned long *pdwErrorCode);
        
        HRESULT ( __stdcall *StartScan )( 
            IScanner * This,
            /* [in] */ unsigned long hSession,
            /* [out] */ unsigned long *pdwErrorCode);
        
        HRESULT ( __stdcall *StopScan )( 
            IScanner * This,
            /* [in] */ unsigned long hSession,
            /* [out] */ unsigned long *pdwErrorCode);
        
        HRESULT ( __stdcall *ReadScan )( 
            IScanner * This,
            /* [in] */ unsigned long hSession,
            /* [out] */ unsigned char *pbyBuffer,
            /* [in] */ unsigned long dwBytes,
            /* [out] */ unsigned long *pdwErrorCode);
        
        HRESULT ( __stdcall *ReadScanEx )( 
            IScanner * This,
            /* [in] */ unsigned long hSession,
            /* [out] */ unsigned char *pbyBuffer,
            /* [in] */ unsigned long dwBytes,
            /* [out] */ unsigned long *pdwBytesRead,
            /* [out] */ unsigned long *pdwErrorCode);
        
        END_INTERFACE
    } IScannerVtbl;

    interface IScanner
    {
        CONST_VTBL struct IScannerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IScanner_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IScanner_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IScanner_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IScanner_OpenScannerSession(This,pszDevName,dwDevIOType,phSession,pdwErrorCode)	\
    ( (This)->lpVtbl -> OpenScannerSession(This,pszDevName,dwDevIOType,phSession,pdwErrorCode) ) 

#define IScanner_CloseScannerSession(This,hSession,pdwErrorCode)	\
    ( (This)->lpVtbl -> CloseScannerSession(This,hSession,pdwErrorCode) ) 

#define IScanner_LockScanner(This,hSession,wLockType,dwTimeout,pdwErrorCode)	\
    ( (This)->lpVtbl -> LockScanner(This,hSession,wLockType,dwTimeout,pdwErrorCode) ) 

#define IScanner_UnlockScanner(This,hSession,pdwErrorCode)	\
    ( (This)->lpVtbl -> UnlockScanner(This,hSession,pdwErrorCode) ) 

#define IScanner_GetLampStatus(This,hSession,pbLampOn,pdwErrorCode)	\
    ( (This)->lpVtbl -> GetLampStatus(This,hSession,pbLampOn,pdwErrorCode) ) 

#define IScanner_LampOn(This,hSession,pdwErrorCode)	\
    ( (This)->lpVtbl -> LampOn(This,hSession,pdwErrorCode) ) 

#define IScanner_LampOff(This,hSession,pdwErrorCode)	\
    ( (This)->lpVtbl -> LampOff(This,hSession,pdwErrorCode) ) 

#define IScanner_GetScannerCapabilities(This,hSession,pScannerCapabilities,pdwErrorCode)	\
    ( (This)->lpVtbl -> GetScannerCapabilities(This,hSession,pScannerCapabilities,pdwErrorCode) ) 

#define IScanner_GetBufferInfo(This,hSession,pScanParam,pBufferInfo,pdwdErrorCode)	\
    ( (This)->lpVtbl -> GetBufferInfo(This,hSession,pScanParam,pBufferInfo,pdwdErrorCode) ) 

#define IScanner_InitializeScanner(This,hSession,pdwErrorCode)	\
    ( (This)->lpVtbl -> InitializeScanner(This,hSession,pdwErrorCode) ) 

#define IScanner_ResetScanner(This,hSession,pdwErrorCode)	\
    ( (This)->lpVtbl -> ResetScanner(This,hSession,pdwErrorCode) ) 

#define IScanner_SetScannerParameters(This,hSession,pScanParam,pdwErrorCode)	\
    ( (This)->lpVtbl -> SetScannerParameters(This,hSession,pScanParam,pdwErrorCode) ) 

#define IScanner_SetGammaTable(This,hSession,uSz,pbyRed,pbyGreen,pbyBlue,pdwErrorCode)	\
    ( (This)->lpVtbl -> SetGammaTable(This,hSession,uSz,pbyRed,pbyGreen,pbyBlue,pdwErrorCode) ) 

#define IScanner_StartScan(This,hSession,pdwErrorCode)	\
    ( (This)->lpVtbl -> StartScan(This,hSession,pdwErrorCode) ) 

#define IScanner_StopScan(This,hSession,pdwErrorCode)	\
    ( (This)->lpVtbl -> StopScan(This,hSession,pdwErrorCode) ) 

#define IScanner_ReadScan(This,hSession,pbyBuffer,dwBytes,pdwErrorCode)	\
    ( (This)->lpVtbl -> ReadScan(This,hSession,pbyBuffer,dwBytes,pdwErrorCode) ) 

#define IScanner_ReadScanEx(This,hSession,pbyBuffer,dwBytes,pdwBytesRead,pdwErrorCode)	\
    ( (This)->lpVtbl -> ReadScanEx(This,hSession,pbyBuffer,dwBytes,pdwBytesRead,pdwErrorCode) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IScanner_INTERFACE_DEFINED__ */


#ifndef __ILog_INTERFACE_DEFINED__
#define __ILog_INTERFACE_DEFINED__

/* interface ILog */
/* [object][helpstring][uuid] */ 


EXTERN_C const IID IID_ILog;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("EB0D7369-5CB0-437E-B61B-1B6A4D8538C0")
    ILog : public IUnknown
    {
    public:
        virtual HRESULT __stdcall LogString( 
            /* [in] */ unsigned long dwFunctionAttribute,
            /* [in] */ LPWSTR pszString) = 0;
        
        virtual HRESULT __stdcall IsEnabled( 
            /* [in] */ unsigned long dwFunctionAttribute,
            /* [out] */ unsigned char *pbyEnabled) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct ILogVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ILog * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ILog * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ILog * This);
        
        HRESULT ( __stdcall *LogString )( 
            ILog * This,
            /* [in] */ unsigned long dwFunctionAttribute,
            /* [in] */ LPWSTR pszString);
        
        HRESULT ( __stdcall *IsEnabled )( 
            ILog * This,
            /* [in] */ unsigned long dwFunctionAttribute,
            /* [out] */ unsigned char *pbyEnabled);
        
        END_INTERFACE
    } ILogVtbl;

    interface ILog
    {
        CONST_VTBL struct ILogVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ILog_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ILog_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ILog_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ILog_LogString(This,dwFunctionAttribute,pszString)	\
    ( (This)->lpVtbl -> LogString(This,dwFunctionAttribute,pszString) ) 

#define ILog_IsEnabled(This,dwFunctionAttribute,pbyEnabled)	\
    ( (This)->lpVtbl -> IsEnabled(This,dwFunctionAttribute,pbyEnabled) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ILog_INTERFACE_DEFINED__ */


#ifndef __IButton_INTERFACE_DEFINED__
#define __IButton_INTERFACE_DEFINED__

/* interface IButton */
/* [object][helpstring][uuid] */ 


EXTERN_C const IID IID_IButton;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("B59EE25B-8A4A-4798-8BB4-AE7AD33FBD4F")
    IButton : public IUnknown
    {
    public:
        virtual HRESULT __stdcall GetButtonCount( 
            /* [in] */ unsigned long hSession,
            /* [out] */ unsigned short *pwButtonCount,
            /* [out] */ unsigned long *pdwErrorCode) = 0;
        
        virtual HRESULT __stdcall GetButtonState( 
            /* [in] */ unsigned long hSession,
            /* [in] */ unsigned char byIndex,
            /* [out] */ unsigned char *pbyState,
            /* [out] */ unsigned long *pdwErrorCode) = 0;
        
        virtual HRESULT __stdcall GetButtonStateEx( 
            /* [in] */ unsigned long hSession,
            /* [in] */ unsigned char byCount,
            /* [out] */ unsigned char *pState,
            /* [out] */ unsigned long *pdwErrorCode) = 0;
        
        virtual HRESULT __stdcall ResetButton( 
            /* [in] */ unsigned long hSession,
            /* [in] */ unsigned char byIndex,
            /* [out] */ unsigned long *pdwErrorCode) = 0;
        
        virtual HRESULT __stdcall ResetButtonsEx( 
            /* [in] */ unsigned long hSession,
            /* [out] */ unsigned long *pdwErrorCode) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IButtonVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IButton * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IButton * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IButton * This);
        
        HRESULT ( __stdcall *GetButtonCount )( 
            IButton * This,
            /* [in] */ unsigned long hSession,
            /* [out] */ unsigned short *pwButtonCount,
            /* [out] */ unsigned long *pdwErrorCode);
        
        HRESULT ( __stdcall *GetButtonState )( 
            IButton * This,
            /* [in] */ unsigned long hSession,
            /* [in] */ unsigned char byIndex,
            /* [out] */ unsigned char *pbyState,
            /* [out] */ unsigned long *pdwErrorCode);
        
        HRESULT ( __stdcall *GetButtonStateEx )( 
            IButton * This,
            /* [in] */ unsigned long hSession,
            /* [in] */ unsigned char byCount,
            /* [out] */ unsigned char *pState,
            /* [out] */ unsigned long *pdwErrorCode);
        
        HRESULT ( __stdcall *ResetButton )( 
            IButton * This,
            /* [in] */ unsigned long hSession,
            /* [in] */ unsigned char byIndex,
            /* [out] */ unsigned long *pdwErrorCode);
        
        HRESULT ( __stdcall *ResetButtonsEx )( 
            IButton * This,
            /* [in] */ unsigned long hSession,
            /* [out] */ unsigned long *pdwErrorCode);
        
        END_INTERFACE
    } IButtonVtbl;

    interface IButton
    {
        CONST_VTBL struct IButtonVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IButton_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IButton_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IButton_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IButton_GetButtonCount(This,hSession,pwButtonCount,pdwErrorCode)	\
    ( (This)->lpVtbl -> GetButtonCount(This,hSession,pwButtonCount,pdwErrorCode) ) 

#define IButton_GetButtonState(This,hSession,byIndex,pbyState,pdwErrorCode)	\
    ( (This)->lpVtbl -> GetButtonState(This,hSession,byIndex,pbyState,pdwErrorCode) ) 

#define IButton_GetButtonStateEx(This,hSession,byCount,pState,pdwErrorCode)	\
    ( (This)->lpVtbl -> GetButtonStateEx(This,hSession,byCount,pState,pdwErrorCode) ) 

#define IButton_ResetButton(This,hSession,byIndex,pdwErrorCode)	\
    ( (This)->lpVtbl -> ResetButton(This,hSession,byIndex,pdwErrorCode) ) 

#define IButton_ResetButtonsEx(This,hSession,pdwErrorCode)	\
    ( (This)->lpVtbl -> ResetButtonsEx(This,hSession,pdwErrorCode) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IButton_INTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_HP3300;

#ifdef __cplusplus

class DECLSPEC_UUID("F20FB4D8-0C85-454B-AA3F-4F0E803A0E5D")
HP3300;
#endif
#endif /* __HP3300Lib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


