
#include "stdafx.h"
#include "app.h"
#include "dialog.h"
#include "httpprotocol.h"
#include "log.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define FEATURE_OWN_PROTOCOL 1

BEGIN_MESSAGE_MAP( App, CWinApp )
    ON_COMMAND( ID_HELP, &CWinApp::OnHelp )
END_MESSAGE_MAP()

App::App() {
    m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;
}

App theApp;

BOOL App::InitInstance() {

    LOG( "\n" );
    LOG( "####    Starting Browser    ####" );

#if FEATURE_OWN_PROTOCOL
    // register http and https
    bool ok = CHttpProtocolFactory::RegisterFactory();
    if( !ok ) {
        LOG_ERROR( "Could not register own protocol!" );
    }
#endif // FEATURE_OWN_PROTOCOL

    this->initXP();

    CWinApp::InitInstance();

    AfxEnableControlContainer();

    Dialog dlg;
    m_pMainWnd = &dlg;
    INT_PTR nResponse = dlg.DoModal();

    if( nResponse == IDOK ) {
        LOG( "Dialog is IDOK" );
    } else if( nResponse == IDCANCEL ) {
        LOG( "Dialog is IDCANCEL" );
    } else if( nResponse == -1 ) {
        TRACE( traceAppMsg, 0, "Warning: dialog creation failed, so application is terminating unexpectedly.\n" );
        TRACE( traceAppMsg, 0, "Warning: if you are using MFC controls on the dialog, you cannot #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS.\n" );
    }

    // Since the dialog has been closed, return FALSE so that we exit the
    //  application, rather than start the application's message pump.
    return FALSE;
}

int App::ExitInstance() {
    LOG( "####    Stopping Browser    ####" );
    LOG( "\n" );
    return CWinApp::ExitInstance();
}

// InitCommonControlsEx() is required on Windows XP if an application
// manifest specifies use of ComCtl32.dll version 6 or later to enable
// visual styles.  Otherwise, any window creation will fail.
void App::initXP() {
    INITCOMMONCONTROLSEX InitCtrls;
    InitCtrls.dwSize = sizeof( InitCtrls );
    // Set this to include all the common control classes you want to use
    // in your application.
    InitCtrls.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx( &InitCtrls );
}

