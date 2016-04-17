// DesktopID.cpp : implementation file
//

#include "stdafx.h"
#include "VDeskSwitcher.h"
#include "DesktopID.h"
#include "afxdialogex.h"


// DesktopID dialog

IMPLEMENT_DYNAMIC(DesktopID, CDialogEx)

DesktopID::DesktopID(DWORD DeskID, CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DesktopId, pParent)
{
	ID = DeskID;
}

DesktopID::~DesktopID()
{
}

void DesktopID::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(DesktopID, CDialogEx)
	ON_WM_TIMER()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()


// DesktopID message handlers
BOOL DesktopID::OnInitDialog() {
	WINDOWPLACEMENT WP;
	POINT ptMinPos;
	POINT ptMaxPos;
	RECT rcNormalPos;
	CStatic *sID;
	CString szID;

	ptMinPos.x = 0;
	ptMinPos.y = 0;
	ptMaxPos.x = 0;
	ptMaxPos.y = 0;

	rcNormalPos.left = GetSystemMetrics(SM_XVIRTUALSCREEN);
	rcNormalPos.top = GetSystemMetrics(SM_YVIRTUALSCREEN);
	rcNormalPos.right = GetSystemMetrics(SM_CXVIRTUALSCREEN) + rcNormalPos.left;
	rcNormalPos.bottom = GetSystemMetrics(SM_CYVIRTUALSCREEN) + rcNormalPos.top;

	WP.length = sizeof(WINDOWPLACEMENT);
	WP.flags = WPF_RESTORETOMAXIMIZED;
	WP.showCmd = SW_SHOWMAXIMIZED;
	WP.ptMinPosition = ptMinPos;
	WP.ptMaxPosition = ptMaxPos;
	WP.rcNormalPosition = rcNormalPos;
	SetWindowPlacement(&WP);

	szID.Format(_T("%d"), ID);
	sID = reinterpret_cast<CStatic *>(GetDlgItem(IDC_DesktopID));
	sID->SetWindowText((LPCTSTR)szID);
	sID->SetWindowPos( NULL, rcNormalPos.left, rcNormalPos.top, rcNormalPos.right, rcNormalPos.bottom, SWP_SHOWWINDOW);

	// TODO: Set colours based on System Metrics
	
	return TRUE;  // return TRUE  unless you set the focus to a control

}

void DesktopID::OnSetFocus(CWnd *pParent) {
	SetTimer(NULL, 100, NULL); // TODO: Make this variable?
}

void DesktopID::OnTimer( UINT_PTR pTimer ) {
	KillTimer(pTimer);
	OnOK();
}
