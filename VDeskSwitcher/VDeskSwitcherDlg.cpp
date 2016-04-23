
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

#ifndef WM_TRAY
#define WM_TRAY (WM_USER+1)
#endif

#if 1
void ErrorMsg(LPTSTR lpszFunction)
{
	// Retrieve the system error message for the last-error code
	LPVOID lpMsgBuf;
	//LPVOID lpDisplayBuf;
	DWORD dw = GetLastError();
	CString DisplayBuf;

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

	DisplayBuf.Format(_T("%s failed with error %d: %s"),
		lpszFunction, dw, lpMsgBuf);
	MessageBox(NULL, (LPCTSTR)DisplayBuf, TEXT("Error"), MB_OK);

	LocalFree(lpMsgBuf);
}
#endif

// CVDeskSwitcherDlg dialog

CVDeskSwitcherDlg::CVDeskSwitcherDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_VDESKSWITCHER_DIALOG, pParent)
	, Delay(0)
	, Enabled(0)
	, StartMinimized(0)
	, PinToDesk(FALSE)
	, Identify(FALSE)
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
	DDX_Check(pDX, IDC_PinToDesk, PinToDesk);
	DDX_Check(pDX, IDC_Identify, Identify);
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
	ON_BN_CLICKED(IDC_PinToDesk, &CVDeskSwitcherDlg::OnBnClickedPintodesk)
	ON_BN_CLICKED(IDC_Identify, &CVDeskSwitcherDlg::OnBnClickedIdentify)
	ON_MESSAGE(WM_TRAY, &CVDeskSwitcherDlg::OnTray)
	ON_WM_ACTIVATE()
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
	FirstInit = TRUE;

	StartMinimized	= A->GetProfileInt(Config, kStartMinimized, 0);
	Enabled		= A->GetProfileInt(Config, kEnabled, 0);
	Delay		= A->GetProfileInt(Config, kDelay, DEF_Delay);
	PinToDesk	= A->GetProfileInt(Config, kPinToDesk, 0);
	Identify	= A->GetProfileInt(Config, kIdentify, 0);
	
	A->WriteProfileInt(Config, kStartMinimized, (BOOL)StartMinimized);
	A->WriteProfileInt(Config, kEnabled, (BOOL)Enabled);
	A->WriteProfileInt(Config, kDelay, Delay);
	A->WriteProfileInt(Config, kPinToDesk, PinToDesk);
	A->WriteProfileInt(Config, kIdentify, Identify);

	Timer = NULL;
	StartTimer(1);
	FirstHit = 0;

	UpdateData(FALSE);

	Tray.cbSize = sizeof(Tray);
	Tray.hWnd = m_hWnd;
	Tray.uID = 1;
	Tray.uFlags = NIF_ICON | NIF_MESSAGE;
	Tray.uCallbackMessage = WM_TRAY;
	Tray.hIcon = A->LoadIconW(IDR_MAINFRAME);
	Tray.dwState = NIS_SHAREDICON;
	Tray.dwInfoFlags = NIIF_USER;
	if (!Shell_NotifyIcon(NIM_ADD, &Tray)) {
		ErrorMsg(_T("Shell_NotifyIcon"));
	}
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

LRESULT CVDeskSwitcherDlg::OnTray(WPARAM w, LPARAM l)
{
	// TODO: for now just show the window, for full feature pop up a menu that will give options.

	// WM_MBUTTONUP is working on middle button click
	// watching messages shows 514=1d, 513=1u, 516,517=2d,u
	
	// l==513,514,515: button 1 down,up,double
	// l==516,517,518: button 2 down,up,double
	// So... swtich((l-513)%3){ case 0 any up; case 1 any down; case 2 any double...  what's the max??
	// l-WM_MOUSEFIRST == 1 ::= any button up
	if (l>WM_MOUSEFIRST && l<=WM_MOUSELAST) {
		switch ((l-WM_MOUSEFIRST-1)%3) {
			case 0:
				//MessageBox(_T("Down"));
				break;
			case 1:
				// ShowMenu(TRAY_MENU);
				// The problem with testing button up is double click gives two button ups followed by a double click...
				
				//MessageBox(_T("Up"));
				break;
			case 2:
				ShowWindow(SW_SHOW);
				//MessageBox(_T("Double"));
				break;
			default:
				// print l;
				break;
		}
	}
	return FALSE;
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

void CVDeskSwitcherDlg::StartTimer(DWORD Time) {
	KillTimer(Timer);
	if (Time == 0) {
		Time = (max(AfxGetApp()->GetProfileInt(Config, kDelay, DEF_Delay) / 5, 10));
	}
	if (Enabled||FirstInit) { Timer = SetTimer(Timer, Time, NULL); }
}

bool IsTopMost(HWND hwnd)
{
	WINDOWINFO info;
	GetWindowInfo(hwnd, &info);
	return (info.dwExStyle & WS_EX_TOPMOST) ? true : false;
}

bool IsFullScreenSize(HWND hwnd, const int cx, const int cy)
{
	RECT r;
	::GetWindowRect(hwnd, &r);
	return r.right - r.left == cx && r.bottom - r.top == cy;
}

bool IsFullscreenAndMaximized(HWND hwnd)
{
	if (IsTopMost(hwnd))
	{
		const int cx = GetSystemMetrics(SM_CXSCREEN);
		const int cy = GetSystemMetrics(SM_CYSCREEN);
		if (IsFullScreenSize(hwnd, cx, cy))
			return TRUE;
	}
	return false;
}

BOOL CALLBACK Maximized(HWND hWnd, LPARAM l) {
	if (IsFullscreenAndMaximized(hWnd))
	{
		*(bool*)l = TRUE;
		return FALSE; //there can be only one so quit here
	}
	return TRUE;
}

BOOL FullScreenApp() {
	BOOL IsFull = FALSE;
	EnumWindows((WNDENUMPROC)Maximized, (LPARAM)&IsFull);
	return(IsFull);
}

BOOL MouseDown() {
	BOOL isDown = FALSE;
	int Buttons[] = {VK_LBUTTON, VK_RBUTTON, VK_MBUTTON, VK_XBUTTON1, VK_XBUTTON2};
	SHORT State = 0;
	int i = 0;
	int last = sizeof(Buttons) / sizeof(int);
	for (i = 0; i < last; i++) {
		State = GetAsyncKeyState(Buttons[i]);
		if(State & (0x1<<15)) isDown = TRUE;
	}
	return(isDown);
}

void CVDeskSwitcherDlg::OnTimer(UINT_PTR pTimer)
{
	POINT prevP = MousePos;
	GetCursorPos(&MousePos);
	DWORD Left = GetSystemMetrics(SM_XVIRTUALSCREEN);
	DWORD Right = GetSystemMetrics(SM_CXVIRTUALSCREEN) + Left - 1;
	DWORD ThisHit = GetTickCount();
	KillTimer( pTimer );
	
	if (FirstInit) {
		FirstInit = FALSE;
		if (StartMinimized) ShowWindow(SW_HIDE);
	}
	
	if (FullScreenApp()) {
		StartTimer(1000);
		return;
	}

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
	else if (FirstHit > 0 && ThisHit >= FirstHit + Delay && !MouseDown()) {
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

void CVDeskSwitcherDlg::PrintDeskID(DWORD id) {
	LONG Left = GetSystemMetrics(SM_XVIRTUALSCREEN);
	LONG Right = GetSystemMetrics(SM_CXVIRTUALSCREEN) + Left;
	LONG Top = GetSystemMetrics(SM_YVIRTUALSCREEN);
	LONG Bottom = GetSystemMetrics(SM_CYVIRTUALSCREEN) + Top;
	CWnd *Overlay = new CWnd();
	CStatic *ID = new CStatic();
	CString sID;
	CRect Screen = {Left, Top, Bottom, Right};
	sID.Format(_T("%d"), id);
	Overlay->Create(_T("STATIC"),\
		_T("VDeskSwitcher Screen ID"),\
		WS_VISIBLE,\
		Screen,\
		this, 65535 );
	ID->Create((LPCTSTR)sID, WS_CHILD|SS_CENTER, Screen, Overlay);
	// TODO: Set Font
	Overlay->ShowWindow(SW_SHOWMAXIMIZED);
	Overlay->SetForegroundWindow();
	Sleep(2500);
	ID->DestroyWindow();
	Overlay->DestroyWindow();
	LocalFree(ID);
	LocalFree(Overlay);
	ID = NULL;
	Overlay = NULL;
	sID.ReleaseBuffer();
}

// http://www.cyberforum.ru/blogs/105416/blog3671.html
void CVDeskSwitcherDlg::ChangeDesktop(DWORD Direction) {
// Maybe move these to the class & their inits to OnInit so it can be used in many functions.  Still a good idea
// to Enum them every so often if VDesks are added/removed after OnInit.
	IServiceProvider* SP = NULL;
	IVirtualDesktopManagerInternal *VDMi = NULL;
	IVirtualDesktopManager *VDM = NULL;
	IVirtualDesktop *D = NULL;
	IVirtualDesktop *nextD = NULL;
	CArray<GUID> Desks;
	GUID nextGUID = {0};
//	MessageBox( _T("Times Up!"));
	
	if (!SUCCEEDED(CoCreateInstance(CLSID_ImmersiveShell, NULL, CLSCTX_LOCAL_SERVER, __uuidof(IServiceProvider), (PVOID*)&SP))) return;
	if (!SUCCEEDED(SP->QueryService(CLSID_VirtualDesktopAPI_Unknown, &VDMi))) return;
	if (!SUCCEEDED(SP->QueryService(__uuidof(IVirtualDesktopManager), &VDM))) return;
	if (!SUCCEEDED(VDMi->GetCurrentDesktop(&D))) return;

	switch (Direction) {
		case VK_LEFT:
			if (!SUCCEEDED(VDMi->GetAdjacentDesktop(D, AdjacentDesktop::LeftDirection, &nextD))) return;
			break;
		case VK_RIGHT:
			if (!SUCCEEDED(VDMi->GetAdjacentDesktop(D, AdjacentDesktop::RightDirection, &nextD))) return;
			break;
	}
	if (!SUCCEEDED(nextD->GetID(&nextGUID))) return;
	
	EnumVirtualDesktops(VDMi, &Desks);
	VDMi->SwitchDesktop(nextD);
	
// The window doesn't pin when switched by keyboard, maybe move this to a fn and call that on timer too, or is that too heavy handed?
	// TODO: fix this...
	if (TRUE) { // if (PinToDesk) { // Identify doesn't work unless Pinned, not sure how this will change when hidden.
		VDM->MoveWindowToDesktop( m_hWnd, nextGUID );
		// CWnd *front = GetForegroundWindow();
		CWnd *me = this;
		if ( GetFocus() == me ) // TODO: Not working
			SetWindowPos(&wndTop, 0,0,0,0, SWP_NOSIZE);
	}
	if (Identify) {
		for (DWORD i = 0; i < Desks.GetCount(); i++) {
			if (nextGUID == Desks[i]) {
				pDID = new DesktopID(i+1);
				VDM->MoveWindowToDesktop(pDID->m_hWnd, nextGUID);
				pDID->DoModal();
			}
		}
	}

	nextD->Release();
	D->Release();
	VDM->Release();
	SP->Release();
	nextD = NULL;
	D = NULL;
	VDMi = NULL;
	SP = NULL;
}


// Normal Event Handlers

void CVDeskSwitcherDlg::OnBnClickedOk()
{
	// Hide the dialog
	ShowWindow(SW_HIDE);
}

void CVDeskSwitcherDlg::OnBnClickedCancel()
{
	// Remove the tray icon
	Shell_NotifyIcon(NIM_DELETE, &Tray);
	CDialogEx::OnOK();
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


void CVDeskSwitcherDlg::OnBnClickedPintodesk()
{
	UpdateData();
	AfxGetApp()->WriteProfileInt(Config, kPinToDesk, PinToDesk);
	UpdateData(FALSE);
}


void CVDeskSwitcherDlg::OnBnClickedIdentify()
{
	UpdateData();
	AfxGetApp()->WriteProfileInt(Config, kIdentify, Identify);
	UpdateData(FALSE);
}
