#pragma once


// DesktopID dialog

class DesktopID : public CDialogEx
{
	DECLARE_DYNAMIC(DesktopID)

public:
	DesktopID(DWORD, CWnd* pParent = NULL);   // standard constructor
	virtual ~DesktopID();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	
private:
	afx_msg void OnSetFocus(CWnd*);
	afx_msg void OnTimer(UINT_PTR);
	DWORD ID;

	DECLARE_MESSAGE_MAP()
};
