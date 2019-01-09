

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0595 */
/* at Mon Apr 07 17:52:24 2014
 */
/* Compiler settings for LogBook.odl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 8.00.0595 
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


#ifndef __LogBook_h_h__
#define __LogBook_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __ILogBook_FWD_DEFINED__
#define __ILogBook_FWD_DEFINED__
typedef interface ILogBook ILogBook;

#endif 	/* __ILogBook_FWD_DEFINED__ */


#ifndef __LogBook_FWD_DEFINED__
#define __LogBook_FWD_DEFINED__

#ifdef __cplusplus
typedef class LogBook LogBook;
#else
typedef struct LogBook LogBook;
#endif /* __cplusplus */

#endif 	/* __LogBook_FWD_DEFINED__ */


#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __LogBook_LIBRARY_DEFINED__
#define __LogBook_LIBRARY_DEFINED__

/* library LogBook */
/* [version][uuid] */ 


DEFINE_GUID(LIBID_LogBook,0x675A5B53,0xC78A,0x11D2,0x9D,0xAF,0x00,0x00,0xE8,0x52,0xC9,0x7A);

#ifndef __ILogBook_DISPINTERFACE_DEFINED__
#define __ILogBook_DISPINTERFACE_DEFINED__

/* dispinterface ILogBook */
/* [uuid] */ 


DEFINE_GUID(DIID_ILogBook,0x675A5B54,0xC78A,0x11D2,0x9D,0xAF,0x00,0x00,0xE8,0x52,0xC9,0x7A);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("675A5B54-C78A-11D2-9DAF-0000E852C97A")
    ILogBook : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct ILogBookVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ILogBook * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ILogBook * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ILogBook * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ILogBook * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ILogBook * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ILogBook * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ILogBook * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        END_INTERFACE
    } ILogBookVtbl;

    interface ILogBook
    {
        CONST_VTBL struct ILogBookVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ILogBook_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ILogBook_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ILogBook_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ILogBook_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define ILogBook_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define ILogBook_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define ILogBook_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __ILogBook_DISPINTERFACE_DEFINED__ */


DEFINE_GUID(CLSID_LogBook,0x675A5B52,0xC78A,0x11D2,0x9D,0xAF,0x00,0x00,0xE8,0x52,0xC9,0x7A);

#ifdef __cplusplus

class DECLSPEC_UUID("675A5B52-C78A-11D2-9DAF-0000E852C97A")
LogBook;
#endif
#endif /* __LogBook_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


