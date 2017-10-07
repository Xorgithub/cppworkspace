// ServerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Server.h"
#include "ServerDlg.h"

#include "Globle.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#pragma comment(lib, "ws2_32.lib")

void Init()
{
	InitializeCriticalSection(&csGeneralData);
	InitializeCriticalSection(&csShowMsg);
	srand(time(0));
}

void BeforeExit()
{
	DeleteCriticalSection(&csGeneralData);
	DeleteCriticalSection(&csShowMsg);
}

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
// CServerDlg dialog

CServerDlg::CServerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CServerDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CServerDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CServerDlg)
	DDX_Control(pDX, IDC_LIST_MSG, m_ctrListMsg);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CServerDlg, CDialog)
	//{{AFX_MSG_MAP(CServerDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CServerDlg message handlers

BOOL CServerDlg::OnInitDialog()
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
	m_iTotalClients = 0;
	m_ctrListMsg.SetHorizontalExtent(1000);		// ����ˮƽ������
	m_ctrListMsg.SetHorizontalExtent(1000);		// ����ˮƽ������

	CString sMsg;
	DWORD dwCode;
	WSADATA wsaData;
	Init();

	dwCode = WSAStartup(MAKEWORD(2,2), &wsaData);
	if(dwCode != 0)
	{
		this->MessageBox("Can't find find a usable WinSock DLL");
		return FALSE;
	}
	if ( LOBYTE( wsaData.wVersion ) != 2 ||  HIBYTE( wsaData.wVersion ) != 2 )
	{
		MessageBox("Can't find the socket version required.");
		return FALSE;
    }

	Listen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(Listen == INVALID_SOCKET)
	{
		sMsg.Format("Can't create the socket, error code%d", WSAGetLastError());
		MessageBox(sMsg);
		return FALSE;
	}

	SOCKADDR_IN sockAddr;
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_addr.s_addr = ADDR_ANY;
	sockAddr.sin_port = htons(PORT);
	dwCode = bind(Listen, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR));
	if(dwCode == SOCKET_ERROR)
	{
		::closesocket(Listen);
		sMsg.Format("Can't bind the socket, error code:%d",WSAGetLastError());
		MessageBox(sMsg);
		return FALSE;
	}
	::WSAAsyncSelect(Listen, GetSafeHwnd(), WM_SOCKET, FD_ACCEPT | FD_CLOSE);
	listen(Listen, 5);

	
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CServerDlg::OnPaint() 
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
HCURSOR CServerDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

LRESULT CServerDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	CString sMsg;
	DWORD	dwCode;
	char Buf[BUFFER_SIZE] = "";

	switch(message)
	{
	case WM_SOCKET:
		if(WSAGETSELECTERROR(lParam))	// ������
		{
			sMsg.Format("Occured an error for socket %d, error code %d", wParam, WSAGETSELECTERROR(lParam));
			closesocket(wParam);
			AddMsg(sMsg);

			m_iTotalClients--;
			break;
		}
		sMsg.Format("Total clients:%d", m_iTotalClients);
		AddMsg(sMsg);
		switch(WSAGETSELECTEVENT(lParam))
		{
		case FD_ACCEPT:
			Accept = ::accept(wParam, NULL, NULL);
			sMsg.Format("Socket %d connected...", Accept);
			AddMsg(sMsg);
			WSAAsyncSelect(Accept, this->GetSafeHwnd(), WM_SOCKET, FD_READ | FD_WRITE | FD_CLOSE);
			m_iTotalClients++;
			break;
		case FD_READ:
			dwCode = recv(wParam, Buf, BUFFER_SIZE, 0);
			if(dwCode == SOCKET_ERROR)
			{
				sMsg.Format("An error occured when reading data from socket %d, error code %d",
							wParam, ::WSAGetLastError());
				AddMsg(sMsg);
				break;
			}
			Buf[dwCode] = '\0';
			sMsg.Format("Received data from socket %d:%s", wParam, Buf);
			AddMsg(sMsg);
		/*	*/
			GetData(Buf);
			dwCode = send(wParam, Buf, BUFFER_SIZE, 0);
			if(dwCode == SOCKET_ERROR)
			{
				sMsg.Format("An error occured when Sending data to socket %d, error code %d", 
							wParam, ::WSAGetLastError());
				AddMsg(sMsg);
				break;
			}
			sMsg.Format("Sended data to socket %d: %s", wParam, Buf);
			AddMsg(sMsg);
		
		case FD_WRITE:
		/*
			GetData(Buf);
			dwCode = send(wParam, Buf, BUFFER_SIZE, 0);
			if(dwCode == SOCKET_ERROR)
			{
				sMsg.Format("An error occured when Sending data to socket %d, error code %d", 
							wParam, ::WSAGetLastError());
				AddMsg(sMsg);
				break;
			}
			sMsg.Format("Sended data to socket %d: %s", wParam, Buf);
			AddMsg(sMsg);
		*/
			break;
		case FD_CLOSE:
			closesocket(wParam);
			sMsg.Format("The connection for socket %d is closed", wParam);
			AddMsg(sMsg);
			m_iTotalClients--;
			break;
		}
		break;
	}

	return CDialog::DefWindowProc(message, wParam, lParam);
}

void CServerDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here
	closesocket(Listen);
	BeforeExit();
}


void CServerDlg::AddMsg(LPCTSTR Msg, BOOL bAddTime)
{
	if(m_ctrListMsg.GetCount() > 300)
		m_ctrListMsg.ResetContent();
	CString sNewMsg(Msg);
	if(bAddTime)
	{
		sNewMsg.Format("%s:%s", CTime::GetCurrentTime().Format("%H:%M:%S"), Msg);
	}
	this->m_ctrListMsg.InsertString(0, sNewMsg);
}
