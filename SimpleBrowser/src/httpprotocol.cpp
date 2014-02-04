
#include"stdafx.h"

#include <vector>
#include <stdio.h>
#include <fstream>
#include <algorithm>

#include "httpprotocol.h"
#include "log.h"

#include "Wininet.h"
#pragma comment(lib, "wininet")

HttpProtocol::HttpProtocol( void ) {
    mResponse = 0;
    refCount = 0;
    mOffset = 0;
    mSink = NULL;
    mNeg = NULL;
}

HttpProtocol::~HttpProtocol( void ) {
}

HRESULT STDMETHODCALLTYPE HttpProtocol::Read( void* pv, ULONG cb, ULONG* pcbRead ) {

    unsigned long toBeRead = max( min( cb, mData.size() - mOffset ), 0 );

    if( toBeRead > 0 ) {
        if( pv ) {
            memcpy( pv, mData.data() + mOffset, toBeRead );
        } else {
            toBeRead = 0;
        }
    }

    mOffset += toBeRead;

    if( pcbRead ) {
        *pcbRead = toBeRead;
    }

    if( toBeRead != 0 ) {
        return S_OK;
    } else {
        return S_FALSE;
    }
}

HRESULT STDMETHODCALLTYPE HttpProtocol::Start( LPCWSTR szUrl, IInternetProtocolSink* pOIProtSink, IInternetBindInfo* pOIBindInfo, DWORD grfPI, HANDLE_PTR dwReserved ) {

    mURL = std::wstring( szUrl );
    mSink = pOIProtSink;
    mSink->AddRef();

    HRESULT hr = S_OK;

    hr = mSink->ReportProgress( BINDSTATUS_FINDINGRESOURCE, szUrl );
    LOGB_ERROR( hr != S_OK, "BINDSTATUS_FINDINGRESOURCE" );

    hr = mSink->ReportProgress( BINDSTATUS_CONNECTING, szUrl );
    LOGB_ERROR( hr != S_OK, "BINDSTATUS_CONNECTING" );

    hr = mSink->ReportProgress( BINDSTATUS_SENDINGREQUEST, szUrl );
    LOGB_ERROR( hr != S_OK, "BINDSTATUS_SENDINGREQUEST" );

    // CComQIPtr<IServiceProvider> provider( mSink );
    // provider->QueryService( IID_IHttpNegotiate, IID_IHttpNegotiate, ( void** ) & mNeg );

    this->Fill();

    this->Finish();

    return S_OK;
}

std::string fromFile( const std::string& filename ) {
    std::ifstream pubFile( filename.c_str() );
    return std::string( ( std::istreambuf_iterator<char>( pubFile ) ), ( std::istreambuf_iterator<char>() ) );
}

void HttpProtocol::Fill() {

    std::wstring wfile = mURL.substr( mURL.find_last_of( L"/" ) + 1 );
    std::string file( wfile.cbegin(), wfile.cend() );

    LOG( "Fetch: " + file );
    mData = fromFile( file );
    mResponse = 200;

    if( file == "simple_css.html" ) {
        mMimeType = L"text/html";
    } else if( file == "simple.css" ) {
        mMimeType = L"text/css";
    }
}

void HttpProtocol::Finish() {

    mSink->AddRef();

    HRESULT hr = S_OK;

    // has no effect:
    // std::wstring header( L"Content-Type : " );
    // header += mMimeType;
    // hr = mNeg->OnResponse( mResponse, header.c_str(), NULL, NULL );
    // LOGB_ERROR( hr != S_OK, "IHttpNegotiate->OnResponse()" );

    hr = mSink->ReportProgress( BINDSTATUS_MIMETYPEAVAILABLE, mMimeType.c_str() );
    LOGB_ERROR( hr != S_OK, "BINDSTATUS_MIMETYPEAVAILABLE" );

    hr = mSink->ReportProgress( BINDSTATUS_VERIFIEDMIMETYPEAVAILABLE, mMimeType.c_str() );
    LOGB_ERROR( hr != S_OK, "BINDSTATUS_VERIFIEDMIMETYPEAVAILABLE" );

    hr = mSink->ReportData( BSCF_FIRSTDATANOTIFICATION, 0, ( ULONG ) mData.length() );
    LOGB_ERROR( hr != S_OK, "BSCF_FIRSTDATANOTIFICATION" );

    hr = mSink->ReportData( BSCF_LASTDATANOTIFICATION, ( ULONG ) mData.length(), ( ULONG ) mData.length() );
    LOGB_ERROR( hr != S_OK, "BSCF_LASTDATANOTIFICATION" );

    hr = mSink->ReportResult( S_OK, mResponse, NULL );
    LOGB_ERROR( hr != S_OK, "ReportResult" );
}

HRESULT STDMETHODCALLTYPE HttpProtocol::Seek(
    /* [in] */ LARGE_INTEGER dlibMove,
    /* [in] */ DWORD dwOrigin,
    /* [out] */ ULARGE_INTEGER* plibNewPosition ) {
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE HttpProtocol::LockRequest(
    /* [in] */ DWORD dwOptions ) {
    return NOERROR;
}

HRESULT STDMETHODCALLTYPE HttpProtocol::UnlockRequest() {
    return NOERROR;
}

HRESULT STDMETHODCALLTYPE HttpProtocol::Continue(
    /* [in] */ PROTOCOLDATA* pProtocolData ) {
    return E_PENDING;
}

void HttpProtocol::Switch() {
    PROTOCOLDATA data = {0};
    mSink->Switch( &data );
}

HRESULT STDMETHODCALLTYPE HttpProtocol::Abort(
    /* [in] */ HRESULT hrReason,
    /* [in] */ DWORD dwOptions ) {
    return INET_E_RESULT_DISPATCHED;
}

HRESULT STDMETHODCALLTYPE HttpProtocol::Terminate( /* [in] */ DWORD dwOptions ) {

    if( mNeg ) {
        mNeg->Release();
        mNeg = NULL;
    }

    if( mSink ) {
        mSink->Release();
        mSink = NULL;
    }

    return NOERROR;
}

HRESULT STDMETHODCALLTYPE HttpProtocol::Suspend( void ) {
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE HttpProtocol::Resume( void ) {
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE HttpProtocol::QueryInterface(
    /* [in] */ REFIID riid,
    /* [iid_is][out] */ void __RPC_FAR* __RPC_FAR* ppvObject ) {
    if( !ppvObject ) {
        return E_NOINTERFACE;
    }

    if( riid == IID_IUnknown ) {
        LOG( "IID_IUnknown" );
        *ppvObject = this;
        AddRef() ;
        return S_OK;
    }

    if( riid == CLSID_GopherProtocol ) {
        LOG( "CLSID_GopherProtocol" );
        *ppvObject = this;
        AddRef() ;
        return S_OK;
    }

    if( riid == CLSID_HttpSProtocol ) {
        LOG( "CLSID_HttpSProtocol" );
        *ppvObject = this;
        AddRef() ;
        return S_OK;
    }

    if( riid == CLSID_HttpProtocol ) {
        LOG( "CLSID_HttpProtocol" );
        *ppvObject = this;
        AddRef() ;
        return S_OK;
    }

    if( riid == CLSID_IEResProtocol ) {
        LOG( "CLSID_IEResProtocol" );
        *ppvObject = this;
        AddRef() ;
        return S_OK;
    }

    *ppvObject = NULL ;
    return E_NOINTERFACE ;
}

ULONG STDMETHODCALLTYPE HttpProtocol::AddRef( void ) {
    return InterlockedIncrement( &refCount ) ;
}

ULONG STDMETHODCALLTYPE HttpProtocol::Release( void ) {
    long nRefCount = 0;
    nRefCount = InterlockedDecrement( &refCount ) ;

    if( nRefCount == 0 ) {
        delete this;
    }

    return nRefCount;
}

//! \brief Register https and http
//! \sa http://msdn.microsoft.com/en-us/library/aa767759%28v=vs.85%29.aspx
bool CHttpProtocolFactory::RegisterFactory() {

    bool success = true;

    static IInternetSession* session = NULL;
    static CHttpProtocolFactory* factory = NULL;

    if( !session ) {
        const HRESULT res = CoInternetGetSession( 0, &session, 0 );

        if( res != S_OK ) {
            success = false;
        }
    }

    if( !factory ) {
        factory = new CHttpProtocolFactory();
    }

    // https
    if( session && factory ) {

        // https
        {
            std::wstring https = L"https";
            const HRESULT res = session->RegisterNameSpace( factory, CLSID_NULL, https.c_str(), 0, 0, 0 );

            if( res != S_OK ) {
                LOG_ERROR( "Could not register https" );
                success = false;
            }
        }

        // gopher
        //{
        //    std::wstring gopher = L"gopher";
        //    const HRESULT res = session->RegisterNameSpace( factory, CLSID_NULL, gopher.c_str(), 0, 0, 0 );

        //    if( res != S_OK ) {
        //        LOG_ERROR( "Could not register gopher" );
        //        success = false;
        //    }
        //}

        // http
        {
            std::wstring http = L"http";
            const HRESULT res = session->RegisterNameSpace( factory, CLSID_NULL, http.c_str(), 0, 0, 0 );

            if( res != S_OK ) {
                LOG_ERROR( "Could not register http" );
                success = false;
            }
        }
    }

    return success;
}

CHttpProtocolFactory::CHttpProtocolFactory() {
    refCount = 0;
}

HRESULT __stdcall CHttpProtocolFactory::CreateInstance( IUnknown* pUnknownOuter, const IID& iid, void** ppv ) {

    if( !ppv ) {
        return E_FAIL;
    }

    *ppv = NULL;

    if( pUnknownOuter != NULL ) {
        return CLASS_E_NOAGGREGATION ;
    }

    HttpProtocol* pObject = new HttpProtocol();

    if( pObject == NULL ) {
        return E_OUTOFMEMORY ;
    }

    HRESULT res = pObject->QueryInterface( iid, ppv );

    if( res != S_OK ) {
        std::wstring comparison;
        {
            comparison.reserve( 1024 );
            HRESULT res = StringFromGUID2( iid, ( LPOLESTR ) comparison.c_str(), 1024 );
        }
    }

    return res;
}

HRESULT __stdcall CHttpProtocolFactory::LockServer( BOOL bLock ) {
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CHttpProtocolFactory::QueryInterface(
    /* [in] */ REFIID riid,
    /* [iid_is][out] */ void __RPC_FAR* __RPC_FAR* ppvObject ) {
    if( !ppvObject ) {
        return E_FAIL;
    }

    if( riid == IID_IUnknown ) {
        *ppvObject = this;
        AddRef();
        return S_OK;
    }

    if( riid == IID_IClassFactory ) {
        *ppvObject = this;
        AddRef();
        return S_OK;
    }

    *ppvObject = NULL;
    return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE CHttpProtocolFactory::AddRef( void ) {
    // threadsafe  usage increment
    return InterlockedIncrement( &refCount );
}

ULONG STDMETHODCALLTYPE CHttpProtocolFactory::Release( void ) {
    long nRefCount = 0;
    // threadsafe  usage decrement
    nRefCount = InterlockedDecrement( &refCount );

    if( nRefCount == 0 ) {
        delete this;
    }

    return nRefCount;
}
