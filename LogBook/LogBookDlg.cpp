// LogBookDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LogBook.h"
#include "LogBookDlg.h"
#include "DlgProxy.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLogBookDlg dialog

IMPLEMENT_DYNAMIC(CLogBookDlg, CDialog);

CLogBookDlg::CLogBookDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLogBookDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLogBookDlg)
	m_OldLogTxt = _T("");
	m_NewLogTxt = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pAutoProxy = NULL;
	GetData(&m_OldLogTxt);
}

CLogBookDlg::~CLogBookDlg()
{
	// If there is an automation proxy for this dialog, set
	//  its back pointer to this dialog to NULL, so it knows
	//  the dialog has been deleted.
	if (m_pAutoProxy != NULL)
		m_pAutoProxy->m_pDialog = NULL;
}

void CLogBookDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLogBookDlg)
	DDX_Text(pDX, IDC_LOG_TXT, m_OldLogTxt);
	DDX_Text(pDX, IDC_LOG_ENTRY, m_NewLogTxt);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CLogBookDlg, CDialog)
	//{{AFX_MSG_MAP(CLogBookDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_CLOSE()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDQUIT, OnQuit)
	ON_BN_CLICKED(ID_SUBMIT_LOG, OnSubmitLog)
	ON_EN_CHANGE(IDC_LOG_TXT, OnChangeLogTxt)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLogBookDlg message handlers

BOOL CLogBookDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CLogBookDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CLogBookDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CLogBookDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

// Automation servers should not exit when a user closes the UI
//  if a controller still holds on to one of its objects.  These
//  message handlers make sure that if the proxy is still in use,
//  then the UI is hidden but the dialog remains around if it
//  is dismissed.

void CLogBookDlg::OnClose() 
{
	if (CanExit())
		CDialog::OnClose();
}

void CLogBookDlg::OnOK() 
{
	if (CanExit())
		CDialog::OnOK();
}

void CLogBookDlg::OnCancel() 
{
	if (CanExit())
		CDialog::OnCancel();
}

BOOL CLogBookDlg::CanExit()
{
	// If the proxy object is still around, then the automation
	//  controller is still holding on to this application.  Leave
	//  the dialog around, but hide its UI.
	if (m_pAutoProxy != NULL)
	{
		ShowWindow(SW_HIDE);
		return FALSE;
	}

	return TRUE;
}



void CLogBookDlg::OnQuit() 
{
	// TODO: Add your control notification handler code here
	CloseFptr();
	OnOK();
}

void CLogBookDlg::OnSubmitLog() 
{
	UpdateData();
	OnEXTSubmitLog(&m_NewLogTxt);
	m_OldLogTxt.Insert(0, (LPCTSTR) m_NewLogTxt);
	m_NewLogTxt.Empty();
	UpdateData(FALSE);
}

void CLogBookDlg::OnChangeLogTxt() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here

}

void CLogBookDlg::OpenFptr()
{
	LogBookFptr = fopen("LogBookFile.log", "a+");
}

void CLogBookDlg::UpdateFptr(CString buffer)
{
	fprintf(LogBookFptr, "%s\n", (LPCTSTR) buffer);
}


void CLogBookDlg::CloseFptr()
{
	fclose(LogBookFptr);
}

void CLogBookDlg::GetData(CString *my_str)
{
	char tmp[500];
	COleDateTime	m_time;

	OpenFptr();

	CString MyTmpStr;
	while(fgets(tmp, 500, LogBookFptr) != NULL)
	{
		if(tmp[0] == '>')
		{
			my_str->Insert(0, (LPCTSTR) MyTmpStr);
			MyTmpStr.Empty();
			my_str->Insert(0, tmp);
			my_str->Insert(0, "\n");
		}
		else
		{
			MyTmpStr += _T(tmp);
		}

	}

	m_time = COleDateTime::GetCurrentTime();

	my_str->Replace("\n", "\r\n");

	CloseFptr();
}

void CLogBookDlg::OnEXTSubmitLog(CString *my_str) 
{
	// TODO: Add your control notification handler code here
	COleDateTime	m_time;
	CString tmp;
	CString ret_str;
	char tmp_str[80];
	
	OpenFptr();

	if (my_str->GetLength() != 0)
	{
		tmp.Empty();
		ret_str.Empty();
		m_time = COleDateTime::GetCurrentTime();

		tmp += m_time.GetCurrentTime().Format(_T("Date = [%d %b %Y], "));
		sprintf(tmp_str, "Time = [%.2d:%.2d]", m_time.GetHour(), m_time.GetMinute());
		tmp += tmp_str;
		
		UpdateFptr(*my_str);

		my_str->Replace("\n", "\r\n");
		tmp.Insert(0, ">");
		UpdateFptr(tmp);

		tmp += "\r\n";
		tmp += *my_str;
		tmp += "\r\n\r\n";

		my_str->Empty();
		*my_str += tmp;
	}

	CloseFptr();
}

