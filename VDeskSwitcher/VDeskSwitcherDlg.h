
// VDeskSwitcherDlg.h : header file
//

#pragma once
#include "DesktopID.h"


// CVDeskSwitcherDlg dialog
class CVDeskSwitcherDlg : public CDialogEx
{
// Construction
public:
	CVDeskSwitcherDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_VDESKSWITCHER_DIALOG };
#endif

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
	
public:
	afx_msg void OnEnUpdateDuration();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedEnabled();
	afx_msg void OnBnClickedStartMinimized();
	afx_msg void ChangeDesktop(DWORD);
	afx_msg void OnTimer(UINT_PTR);
	afx_msg void OnBnClickedPintodesk();
	afx_msg void OnBnClickedIdentify();

private:
	void StartTimer(DWORD=0);
	void PrintDeskID(DWORD);
	LPCTSTR Config = _T("Startup");
	BOOL Enabled;
	LPCTSTR kEnabled = _T("Enabled");
	DWORD Delay;
	LPCTSTR kDelay = _T("Delay");
	BOOL StartMinimized;
	LPCTSTR kStartMinimized = _T("StartMinimized");
	BOOL PinToDesk;
	LPCTSTR kPinToDesk = _T("PinToDesk");
	BOOL Identify;
	LPCTSTR kIdentify = _T("Identify");
	UINT_PTR Timer;
	POINT MousePos;
	DWORD FirstHit;
	DWORD EdgeHit;
	DesktopID *pDID;
	BOOL FirstInit;
	
	NOTIFYICONDATA Tray; 
	afx_msg LRESULT OnTray(WPARAM,LPARAM);
};
