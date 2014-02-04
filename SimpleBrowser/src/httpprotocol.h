#pragma once

#include <string>
#include <functional>
#include <vector>
#include <fstream>
#include <map>
#include <set>
#include <concurrent_queue.h>

#include "urlmon.h"

///////////////////////////////////
// Class-Ids
// Registry-Key: HKEY_CLASSES_ROOT/PROTOCOLS/Handler/
// Res-Protocol: {3050F3BC-98B5-11CF-BB82-00AA00BDCE0B}
static const GUID CLSID_IEResProtocol = { 0x3050F3BC, 0x98B5, 0x11CF, { 0xbb, 0x82, 0x00, 0xaa, 0x00, 0xbd, 0xce, 0x0b } };


//! \see http://www.codeproject.com/Articles/12865/An-Asynchronous-Pluggable-Protocol-Handler-for-dat
//! \see http://support.microsoft.com/default.aspx?scid=kb;EN-US;q260840
class HttpProtocol : public IInternetProtocol {
public:
    HttpProtocol( void );
    ~HttpProtocol( void );

    virtual HRESULT STDMETHODCALLTYPE Read(
        /* [length_is][size_is][out][in] */ void* pv,
        /* [in] */ ULONG cb,
        /* [out] */ ULONG* pcbRead ) ;

    virtual HRESULT STDMETHODCALLTYPE Seek(
        /* [in] */ LARGE_INTEGER dlibMove,
        /* [in] */ DWORD dwOrigin,
        /* [out] */ ULARGE_INTEGER* plibNewPosition ) ;

    virtual HRESULT STDMETHODCALLTYPE LockRequest(
        /* [in] */ DWORD dwOptions ) ;

    virtual HRESULT STDMETHODCALLTYPE UnlockRequest( void );

    virtual HRESULT STDMETHODCALLTYPE Start(
        /* [in] */ LPCWSTR szUrl,
        /* [in] */ IInternetProtocolSink* pOIProtSink,
        /* [in] */ IInternetBindInfo* pOIBindInfo,
        /* [in] */ DWORD grfPI,
        /* [in] */ HANDLE_PTR dwReserved );

    virtual HRESULT STDMETHODCALLTYPE Continue(
        /* [in] */ PROTOCOLDATA* pProtocolData );

    virtual HRESULT STDMETHODCALLTYPE Abort(
        /* [in] */ HRESULT hrReason,
        /* [in] */ DWORD dwOptions );

    virtual HRESULT STDMETHODCALLTYPE Terminate(
        /* [in] */ DWORD dwOptions );

    virtual HRESULT STDMETHODCALLTYPE Suspend( void );

    virtual HRESULT STDMETHODCALLTYPE Resume( void );

    virtual HRESULT STDMETHODCALLTYPE QueryInterface(
        /* [in] */ REFIID riid,
        /* [iid_is][out] */ void __RPC_FAR* __RPC_FAR* ppvObject );

    virtual ULONG STDMETHODCALLTYPE AddRef( void ) ;

    virtual ULONG STDMETHODCALLTYPE Release( void ) ;

public:
    long refCount;

protected:
    IInternetProtocolSink* mSink;
    IHttpNegotiate*         mNeg;
    IInternetBindInfo* mBindInfo;

    int mResponse;
    std::wstring mURL;
    std::string mData;

    off_t mOffset;
    std::wstring mMimeType;

    void Switch();
    void Finish();

    void Fill();
};

class CHttpProtocolFactory : public IClassFactory {

public:
    CHttpProtocolFactory();

    //interface IUnknown methods
    HRESULT __stdcall QueryInterface(
        REFIID riid ,
        void** ppObj );
    ULONG   __stdcall AddRef();
    ULONG   __stdcall Release();


    //interface IClassFactory methods
    HRESULT __stdcall CreateInstance( IUnknown* pUnknownOuter,
                                      const IID& iid,
                                      void** ppv ) ;
    HRESULT __stdcall LockServer( BOOL bLock ) ;

    static bool RegisterFactory();
private:
    long refCount;
};

