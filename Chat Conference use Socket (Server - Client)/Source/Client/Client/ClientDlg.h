// ClientDlg.h : header file
//

#pragma once
#define PORT 25000
#define WM_SOCKET WM_USER+2

// CClientDlg dialog
class CClientDlg : public CDialog
{
	// Construction
public:
	CClientDlg(CWnd* pParent = NULL);	// standard constructor

	// Dialog Data
	enum { IDD = IDD_CLIENT_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


	// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	LRESULT SockMsg(WPARAM wParam,LPARAM lParam);
	char* ConvertToChar(const CString &s);
	void Split(CString src,CString des[3]);
	void mSend(CString Command);
	int mRecv(CString &Command);

	SOCKET sClient;
	sockaddr_in servAdd;
	CString Command;
	int	buffLength;
	CString strResult[3];
	int level;
	int isLogon;

	CString m_msgString;
	afx_msg void OnBnClickedLogin();
	
	CString m_userName;
	afx_msg void OnBnClickedLeave();
	afx_msg void OnEnChangeMsgbox();
	
	CString m_Pass;
	CString m_UserOnline;
	CString m_msgSend;
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedSend();
	CString IP;
	afx_msg void OnBnClickedSignup();
};
