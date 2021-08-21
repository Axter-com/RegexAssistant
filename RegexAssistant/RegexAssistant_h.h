

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.01.0622 */
/* at Mon Jan 18 22:14:07 2038
 */
/* Compiler settings for RegexAssistant.idl:
    Oicf, W1, Zp8, env=Win64 (32b run), target_arch=AMD64 8.01.0622 
    protocol : all , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */



/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 500
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif /* __RPCNDR_H_VERSION__ */


#ifndef __RegexAssistant_h_h__
#define __RegexAssistant_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IRegexAssistant_FWD_DEFINED__
#define __IRegexAssistant_FWD_DEFINED__
typedef interface IRegexAssistant IRegexAssistant;

#endif 	/* __IRegexAssistant_FWD_DEFINED__ */


#ifndef __RegexAssistant_FWD_DEFINED__
#define __RegexAssistant_FWD_DEFINED__

#ifdef __cplusplus
typedef class RegexAssistant RegexAssistant;
#else
typedef struct RegexAssistant RegexAssistant;
#endif /* __cplusplus */

#endif 	/* __RegexAssistant_FWD_DEFINED__ */


#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __RegexAssistant_LIBRARY_DEFINED__
#define __RegexAssistant_LIBRARY_DEFINED__

/* library RegexAssistant */
/* [version][uuid] */ 


EXTERN_C const IID LIBID_RegexAssistant;

#ifndef __IRegexAssistant_DISPINTERFACE_DEFINED__
#define __IRegexAssistant_DISPINTERFACE_DEFINED__

/* dispinterface IRegexAssistant */
/* [uuid] */ 


EXTERN_C const IID DIID_IRegexAssistant;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("1b0815f4-8a77-436b-b931-ed3d53ff6a48")
    IRegexAssistant : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct IRegexAssistantVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IRegexAssistant * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IRegexAssistant * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IRegexAssistant * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IRegexAssistant * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IRegexAssistant * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IRegexAssistant * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IRegexAssistant * This,
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
    } IRegexAssistantVtbl;

    interface IRegexAssistant
    {
        CONST_VTBL struct IRegexAssistantVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IRegexAssistant_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IRegexAssistant_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IRegexAssistant_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IRegexAssistant_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IRegexAssistant_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IRegexAssistant_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IRegexAssistant_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __IRegexAssistant_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_RegexAssistant;

#ifdef __cplusplus

class DECLSPEC_UUID("4422cfd8-7470-47ad-ae16-4b1fd8219fff")
RegexAssistant;
#endif
#endif /* __RegexAssistant_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


