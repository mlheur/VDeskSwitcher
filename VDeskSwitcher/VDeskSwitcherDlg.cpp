
// VDeskSwitcherDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VDeskSwitcher.h"
#include "VDeskSwitcherDlg.h"
#include "afxdialogex.h"
#include "VDM.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#ifndef DEF_Delay
#define DEF_Delay 500
#endif

#if 0
void ErrorMsg(LPTSTR lpszFunction)
{
	// Retrieve the system error message for the last-error code
	LPVOID lpMsgBuf;
	LPVOID lpDisplayBuf;
	DWORD dw = GetLastError();

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0, NULL);

	// Display the error message and exit the process

	lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
		(lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));
	StringCchPrintf((LPTSTR)lpDisplayBuf,
		LocalSize(lpDisplayBuf) / sizeof(TCHAR),
		TEXT("%s failed with error %d: %s"),
		lpszFunction, dw, lpMsgBuf);
	MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);

	LocalFree(lpMsgBuf);
	LocalFree(lpDisplayBuf);
}
#endif

// CVDeskSwitcherDlg dialog

CVDeskSwitcherDlg::CVDeskSwitcherDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_VDESKSWITCHER_DIALOG, pParent)
	, Delay(0)
	, Enabled(0)
	, StartMinimized(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CVDeskSwitcherDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_Duration, Delay);
	DDV_MinMaxUInt(pDX, Delay, 0, 9999);
	DDX_Check(pDX, IDC_Enabled, Enabled);
	DDX_Check(pDX, IDC_StartMinimized, StartMinimized);
}

BEGIN_MESSAGE_MAP(CVDeskSwitcherDlg, CDialogEx) 
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CVDeskSwitcherDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CVDeskSwitcherDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_Enabled, &CVDeskSwitcherDlg::OnBnClickedEnabled)
	ON_BN_CLICKED(IDC_StartMinimized, &CVDeskSwitcherDlg::OnBnClickedStartMinimized)
	ON_EN_UPDATE(IDC_Duration, &CVDeskSwitcherDlg::OnEnUpdateDuration)
	ON_WM_TIMER()
END_MESSAGE_MAP()

// CVDeskSwitcherDlg message handlers
BOOL CVDeskSwitcherDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	CWinApp *A = AfxGetApp();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	StartMinimized	= A->GetProfileInt(Config, kStartMinimized, 0);
	Enabled		= A->GetProfileInt(Config, kEnabled, 0 );
	Delay		= A->GetProfileInt(Config, kDelay, DEF_Delay );
	
	A->WriteProfileInt(Config, kStartMinimized, (BOOL)StartMinimized);
	A->WriteProfileInt(Config, kEnabled, (BOOL)Enabled);
	A->WriteProfileInt(Config, kDelay, Delay);

	Timer = NULL;
	StartTimer();
	FirstHit = 0;

	if (StartMinimized) {
		// Minimize
		// return FALSE;
		UpdateData(FALSE);
	}
	else {
		UpdateData(FALSE);
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CVDeskSwitcherDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	CDialogEx::OnSysCommand(nID, lParam);
}

void CVDeskSwitcherDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);
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
		CDialogEx::OnPaint();
	}
}

void CVDeskSwitcherDlg::OnBnClickedEnabled()
{
	UpdateData();
	AfxGetApp()->WriteProfileInt(Config, kEnabled, (BOOL)Enabled );
	UpdateData(FALSE);
	StartTimer();
	FirstHit = 0;
}

void CVDeskSwitcherDlg::StartTimer() {
	KillTimer(Timer);
	if (Enabled) { Timer = SetTimer(Timer, (max(AfxGetApp()->GetProfileInt(Config, kDelay, DEF_Delay)/5,10)), NULL); }
}

void CVDeskSwitcherDlg::OnTimer(UINT_PTR pTimer)
{
	POINT prevP = MousePos;
	GetCursorPos(&MousePos);
	DWORD Left = GetSystemMetrics(SM_XVIRTUALSCREEN);
	DWORD Right = GetSystemMetrics(SM_CXVIRTUALSCREEN) + Left - 1;
	DWORD ThisHit = GetTickCount();
	KillTimer( pTimer );
	if (FirstHit > 0 && ThisHit < FirstHit + Delay) {
		// Waiting for delay to be reached.
		if ( (MousePos.x != Left && MousePos.x != Right) || (MousePos.x == Left && EdgeHit != Left) || (MousePos.x == Right && EdgeHit != Right) ) {
			FirstHit = 0;
//			MessageBox(_T("Not on edge waiting for time's up"));
		}
		else {
//			MessageBox(_T("Waiting for time's up"));
		}
	}
	else if (FirstHit > 0 && ThisHit >= FirstHit + Delay) {
		if (MousePos.x == Left && prevP.x == MousePos.x && EdgeHit == Left) {
//			MessageBox(_T("Repeated on left edge"));
			ChangeDesktop(VK_LEFT);
			FirstHit = 0;
		}
		else if(MousePos.x == Right && prevP.x == MousePos.x && EdgeHit == Right) {
//			MessageBox(_T("Repeated on right edge"));
			ChangeDesktop(VK_RIGHT);
			FirstHit = 0;
		}
		else {
			FirstHit = 0;
//			MessageBox(_T("Not on edge anymore"));
		}
	}
	else {
		if (MousePos.x == Left && FirstHit == 0) {
			FirstHit = GetTickCount();
//			MessageBox(_T("First on left edge"));
			EdgeHit = Left;
		}
		else if(MousePos.x == Right && FirstHit == 0) {
			FirstHit = GetTickCount();
//			MessageBox(_T("First on right edge"));
			EdgeHit = Right;
		}
		else if (FirstHit > 0) {
			FirstHit = 0;
//			MessageBox(_T("Not on edge at all"));
		}
	}
	StartTimer();
}

void CVDeskSwitcherDlg::ChangeDesktop(DWORD VK) {
	IServiceProvider* SP = NULL;
	IVirtualDesktopManagerInternal *VDM = NULL;
	IVirtualDesktop *D = NULL;
	IVirtualDesktop *nextD = NULL;
	HRESULT hr = 0;
//	MessageBox( _T("Times Up!"));
	
	hr = CoCreateInstance(CLSID_ImmersiveShell, NULL, CLSCTX_LOCAL_SERVER, __uuidof(IServiceProvider), (PVOID*)&SP);
	if (!SUCCEEDED(hr)) return;

	hr = SP->QueryService(CLSID_VirtualDesktopAPI_Unknown, &VDM);
	if (!SUCCEEDED(hr)) return;

	hr = VDM->GetCurrentDesktop(&D);
	if (!SUCCEEDED(hr)) return;

	switch (VK) {
		case VK_LEFT:
			hr = VDM->GetAdjacentDesktop(D, AdjacentDesktop::LeftDirection, &nextD);
			break;
		case VK_RIGHT:
			hr = VDM->GetAdjacentDesktop(D, AdjacentDesktop::RightDirection, &nextD);
			break;
	}
	if (!SUCCEEDED(hr)) return;
	
	VDM->SwitchDesktop(nextD);

	nextD->Release();
	D->Release();
	VDM->Release();
	SP->Release();
	nextD = NULL;
	D = NULL;
	VDM = NULL;
	SP = NULL;
}


// Normal Event Handlers

void CVDeskSwitcherDlg::OnBnClickedOk()
{
	CDialogEx::OnOK();
}

void CVDeskSwitcherDlg::OnBnClickedCancel()
{
	CDialogEx::OnCancel();
}

void CVDeskSwitcherDlg::OnBnClickedStartMinimized()
{
	UpdateData();
	AfxGetApp()->WriteProfileInt(Config, kStartMinimized, (BOOL)StartMinimized);
	UpdateData(FALSE);
}

void CVDeskSwitcherDlg::OnEnUpdateDuration()
{
	UpdateData();
	AfxGetApp()->WriteProfileInt(Config, kDelay, Delay);
	UpdateData(FALSE);
}
