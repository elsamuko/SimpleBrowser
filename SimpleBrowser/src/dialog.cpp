
#include "stdafx.h"
#include "app.h"
#include "dialog.h"
#include "afxdialogex.h"

#include "log.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


Dialog::Dialog( CWnd* pParent /*=NULL*/ )
    : CDialogEx( Dialog::IDD, pParent ) {
    m_hIcon = AfxGetApp()->LoadIcon( IDR_MAINFRAME );
}

void Dialog::DoDataExchange( CDataExchange* pDX ) {
    CDialogEx::DoDataExchange( pDX );
    DDX_Control( pDX, IDC_EXPLORER1, mExplorer );
}

void setAndCheckFeature( INTERNETFEATURELIST feature ) {
    HRESULT hr;
    hr = CoInternetSetFeatureEnabled( feature, SET_FEATURE_ON_PROCESS, true );
    LOGB_ERROR( hr != S_OK, "Feature could not be set : " + std::to_string( feature )  );
    hr = CoInternetIsFeatureEnabled( feature, SET_FEATURE_ON_PROCESS );
    LOGB_ERROR( hr != S_OK, "Feature is not set : " + std::to_string( feature )  );
}

BOOL Dialog::OnInitDialog() {
    CDialogEx::OnInitDialog();

    // has no effect:
    // LOG( "Initializing Internet Explorer Security Settings ..." );
    // setAndCheckFeature( FEATURE_MIME_HANDLING );
    // setAndCheckFeature( FEATURE_MIME_SNIFFING );

    // visit test site
    mExplorer.Navigate( L"http://127.0.0.1:8890/internal/simple_css.html", NULL, NULL, NULL, NULL );

    return TRUE;
}



