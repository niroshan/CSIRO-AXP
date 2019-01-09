// DlgProxy.h : header file
//

#if !defined(AFX_DLGPROXY_H__675A5B5B_C78A_11D2_9DAF_0000E852C97A__INCLUDED_)
#define AFX_DLGPROXY_H__675A5B5B_C78A_11D2_9DAF_0000E852C97A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CLogBookDlg;

/////////////////////////////////////////////////////////////////////////////
// CLogBookDlgAutoProxy command target

class CLogBookDlgAutoProxy : public CCmdTarget
{
	DECLARE_DYNCREATE(CLogBookDlgAutoProxy)

	CLogBookDlgAutoProxy();           // protected constructor used by dynamic creation

// Attributes
public:
	CLogBookDlg* m_pDialog;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLogBookDlgAutoProxy)
	public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CLogBookDlgAutoProxy();

	// Generated message map functions
	//{{AFX_MSG(CLogBookDlgAutoProxy)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	DECLARE_OLECREATE(CLogBookDlgAutoProxy)

	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CLogBookDlgAutoProxy)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGPROXY_H__675A5B5B_C78A_11D2_9DAF_0000E852C97A__INCLUDED_)
