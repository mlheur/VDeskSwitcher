
// VDeskSwitcherDlg.h : header file
//

#pragma once


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
	//afx_msg void OnMouseMove(UINT, CPoint);
	afx_msg void OnMouseLeave();
	afx_msg void ChangeDesktop(DWORD);
	afx_msg void OnTimer(UINT_PTR);

private:
	void StartTimer();
	DWORD Delay;
	DWORD FirstHit;
	DWORD EdgeHit;
	BOOL StartMinimized;
	BOOL Enabled;
	POINT MousePos;
	LPCTSTR Config = _T("Startup");
	LPCTSTR kDelay = _T("Delay");
	LPCTSTR kEnabled = _T("Enabled");
	LPCTSTR kStartMinimized = _T("StartMinimized");
	UINT_PTR Timer;
};
