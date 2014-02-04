
#pragma once
#include "explorer.h"

class Dialog : public CDialogEx {
public:
    Dialog( CWnd* pParent = NULL );  // standard constructor
    enum { IDD = IDD_SIMPLEBROWSER_DIALOG };

protected:
    virtual void DoDataExchange( CDataExchange* pDX );  // DDX/DDV support

protected:
    HICON m_hIcon;
    virtual BOOL OnInitDialog();
public:
    // webbrowser control
    CExplorer mExplorer;
};

