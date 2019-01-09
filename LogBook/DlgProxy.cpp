// DlgProxy.cpp : implementation file
//

#include "stdafx.h"
#include "LogBook.h"
#include "DlgProxy.h"
#include "LogBookDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLogBookDlgAutoProxy

IMPLEMENT_DYNCREATE(CLogBookDlgAutoProxy, CCmdTarget)

CLogBookDlgAutoProxy::CLogBookDlgAutoProxy()
{
	EnableAutomation();
	
	// To keep the application running as long as an automation 
	//	object is active, the constructor calls AfxOleLockApp.
	AfxOleLockApp();

	// Get access to the dialog through the application's
	//  main window pointer.  Set the proxy's internal pointer
	//  to point to the dialog, and set the dialog's back pointer to
	//  this proxy.
	ASSERT (AfxGetApp()->m_pMainWnd != NULL);
	ASSERT_VALID (AfxGetApp()->m_pMainWnd);
	ASSERT_KINDOF(CLogBookDlg, AfxGetApp()->m_pMainWnd);
	m_pDialog = (CLogBookDlg*) AfxGetApp()->m_pMainWnd;
	m_pDialog->m_pAutoProxy = this;
}

CLogBookDlgAutoProxy::~CLogBookDlgAutoProxy()
{
	// To terminate the application when all objects created with
	// 	with automation, the destructor calls AfxOleUnlockApp.
	//  Among other things, this will destroy the main dialog
	if (m_pDialog != NULL)
		m_pDialog->m_pAutoProxy = NULL;
	AfxOleUnlockApp();
}

void CLogBookDlgAutoProxy::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CCmdTarget::OnFinalRelease();
}

BEGIN_MESSAGE_MAP(CLogBookDlgAutoProxy, CCmdTarget)
	//{{AFX_MSG_MAP(CLogBookDlgAutoProxy)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CLogBookDlgAutoProxy, CCmdTarget)
	//{{AFX_DISPATCH_MAP(CLogBookDlgAutoProxy)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_ILogBook to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {675A5B54-C78A-11D2-9DAF-0000E852C97A}
static const IID IID_ILogBook =
{ 0x675a5b54, 0xc78a, 0x11d2, { 0x9d, 0xaf, 0x0, 0x0, 0xe8, 0x52, 0xc9, 0x7a } };

BEGIN_INTERFACE_MAP(CLogBookDlgAutoProxy, CCmdTarget)
	INTERFACE_PART(CLogBookDlgAutoProxy, IID_ILogBook, Dispatch)
END_INTERFACE_MAP()

// The IMPLEMENT_OLECREATE2 macro is defined in StdAfx.h of this project
// {675A5B52-C78A-11D2-9DAF-0000E852C97A}
IMPLEMENT_OLECREATE2(CLogBookDlgAutoProxy, "LogBook.Application", 0x675a5b52, 0xc78a, 0x11d2, 0x9d, 0xaf, 0x0, 0x0, 0xe8, 0x52, 0xc9, 0x7a)

/////////////////////////////////////////////////////////////////////////////
// CLogBookDlgAutoProxy message handlers
