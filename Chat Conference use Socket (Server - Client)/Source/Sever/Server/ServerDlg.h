// ServerDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "stdlib.h"
#include "time.h"
#include<fstream>
#include <vector>
using namespace std;
#define PORT 25000
#define WM_SOCKET WM_USER+1

// CServerDlg dialog
class CServerDlg : public CDialog
{
	// Construction
public:
	CServerDlg(CWnd* pParent = NULL);	// standard constructor

	// Dialog Data
	enum { IDD = IDD_SERVER_DIALOG };

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
	LRESULT SockMsg(WPARAM wParam, LPARAM lParam);
	char* ConvertToChar(const CString &s);
	void Split(CString src,CString des[3]);
	void mSend(SOCKET sk, CString Command);
	int mRecv(SOCKET sk, CString &Command);

	struct SockName
	{

		SOCKET sockClient;
		char Name[200];
	};

	SOCKET sockServer,sockClient,flag,sclient;
	struct sockaddr_in serverAdd;
	int msgType;
	int buffLength,t,lenguser,flagsend,kq, count_sock;
	int number_Socket;
	int first_send;
	SockName *pSock;
	CString strResult[3];
	CString Command;


	CString m_msgString;
	afx_msg void OnBnClickedListen();
	afx_msg void OnBnClickedCancel();

	int checkLogin(char* Name, char* Pass);
	int checkUserName(char* Name);
	int checkSignUp(char* Name);
	int updateData(char* Name, char* Pass);
	int loadData();

	CString ManagedClient;//Save ManagedClient
	CString OnlineClient;//Save OnlineClient
	CString v_IP;//IP: 127.0.0.1
	CString v_Port;//Port: 25000
	afx_msg void OnEnChangeBoxchat();
	vector <string> vtManagedClient;
};
