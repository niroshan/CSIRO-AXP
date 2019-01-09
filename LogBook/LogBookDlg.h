// LogBookDlg.h : header file
//

#if !defined(AFX_LOGBOOKDLG_H__675A5B59_C78A_11D2_9DAF_0000E852C97A__INCLUDED_)
#define AFX_LOGBOOKDLG_H__675A5B59_C78A_11D2_9DAF_0000E852C97A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CLogBookDlgAutoProxy;

/////////////////////////////////////////////////////////////////////////////
// CLogBookDlg dialog

class CLogBookDlg : public CDialog
{
	DECLARE_DYNAMIC(CLogBookDlg);
	friend class CLogBookDlgAutoProxy;

// Construction
public:
	CLogBookDlg(CWnd* pParent = NULL);	// standard constructor
	virtual ~CLogBookDlg();

// Dialog Data
	//{{AFX_DATA(CLogBookDlg)
	enum { IDD = IDD_LOGBOOK_DIALOG };
	CString	m_OldLogTxt;
	CString	m_NewLogTxt;
	//}}AFX_DATA
	bool updateold;
	FILE *LogBookFptr;

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLogBookDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

public:
	void OpenFptr(void);
	void CloseFptr(void);
	void UpdateFptr(CString buffer);
	void CLogBookDlg::GetData(CString *my_str);
	void CLogBookDlg::OnEXTSubmitLog(CString *my_str); 

// Implementation
protected:
	CLogBookDlgAutoProxy* m_pAutoProxy;
	HICON m_hIcon;

	BOOL CanExit();

	// Generated message map functions
	//{{AFX_MSG(CLogBookDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnClose();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	afx_msg void OnQuit();
	afx_msg void OnSubmitLog();
	afx_msg void OnChangeLogTxt();
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOGBOOKDLG_H__675A5B59_C78A_11D2_9DAF_0000E852C97A__INCLUDED_)
