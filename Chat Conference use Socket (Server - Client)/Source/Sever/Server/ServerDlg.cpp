// ServerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Server.h"
#include "ServerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CServerDlg dialog

CServerDlg::CServerDlg(CWnd* pParent /*=NULL*/)
: CDialog(CServerDlg::IDD, pParent)
, m_msgString(_T(""))
, ManagedClient(_T(""))
, OnlineClient(_T(""))
, v_IP(_T(""))
, v_Port(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_BOXCHAT, m_msgString);
	DDX_Text(pDX, IDC_MANAGE, ManagedClient);
	DDX_Text(pDX, IDC_ONLINE, OnlineClient);
	DDX_Text(pDX, IDC_IP, v_IP);
	DDX_Text(pDX, IDC_PORT, v_Port);
}

BEGIN_MESSAGE_MAP(CServerDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_SOCKET,SockMsg)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_LISTEN, &CServerDlg::OnBnClickedListen)
	ON_BN_CLICKED(IDC_CANCEL, &CServerDlg::OnBnClickedCancel)
	//ON_EN_CHANGE(IDC_BOXCHAT, &CServerDlg::OnEnChangeBoxchat)
	ON_EN_CHANGE(IDC_BOXCHAT, &CServerDlg::OnEnChangeBoxchat)
END_MESSAGE_MAP()


// CServerDlg message handlers

BOOL CServerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	// TODO: Add extra initialization here
	v_IP = "127  .  0  .  0  .  1";
	v_Port = "25000";
	UpdateData(FALSE);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CServerDlg::OnPaint()
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
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CServerDlg::Split(CString src, CString des[3])
{
	int p1,p2,p3;

	p1=src.Find(_T("\r\n"),0);
	des[0]=src.Mid(0,p1);
	
	p2=src.Find(_T("\r\n"),p1 + 1);
	des[1] = src.Mid(p1 + 2, p2 - (p1 + 2));

	p3 = src.Find(_T("\r\n"), p2 + 1);
	des[2] = src.Mid(p2 + 2, p3 - (p2 + 2));
}

char* CServerDlg::ConvertToChar(const CString &s)
{
	int nSize = s.GetLength();
	char *pAnsiString = new char[nSize+1];
	memset(pAnsiString,0,nSize+1);
	wcstombs(pAnsiString, s, nSize+1);
	return pAnsiString;
}

void CServerDlg::mSend(SOCKET sk, CString Command)
{
	int Len=Command.GetLength();
	Len+=Len;
	PBYTE sendBuff = new BYTE[1000];
	memset(sendBuff,0,1000);
	memcpy(sendBuff,(PBYTE)(LPCTSTR)Command, Len);
	send(sk,(char*)&Len,sizeof(Len),0);
	send(sk,(char*)sendBuff,Len,0);
	delete sendBuff;
}

int CServerDlg::mRecv(SOCKET sk, CString &Command)
{
	PBYTE buffer = new BYTE[1000];
	memset(buffer,0, 1000);
	recv(sk ,(char*)&buffLength,sizeof(int),0);
	recv(sk,(char*)buffer,buffLength,0);
	TCHAR* ttc = (TCHAR*)buffer;
	Command = ttc;

	if(Command.GetLength()==0)
		return -1;
	return 0;

}

void CServerDlg::OnBnClickedListen()
{
	// TODO: Add your control notification handler code here
	ManagedClient += _T("---------START SERVER---------\r\n");
	loadData();

	sockServer =socket(AF_INET,SOCK_STREAM,0);
	serverAdd.sin_family=AF_INET;
	serverAdd.sin_port=htons(PORT);
	serverAdd.sin_addr.s_addr =htonl(INADDR_ANY);

	int err2=bind(sockServer,(SOCKADDR*)&serverAdd,sizeof (serverAdd));
	int err1=listen(sockServer,5);
	int err =WSAAsyncSelect(sockServer,m_hWnd,WM_SOCKET,FD_READ|FD_ACCEPT|FD_CLOSE);
	if (err)
		MessageBox((LPCTSTR)"Can't call WSAAsyncSelect");	
	number_Socket=0;
	pSock = new SockName[200];
	ManagedClient += _T("----------------------------------------\r\n");
	GetDlgItem(IDC_LISTEN)->EnableWindow(FALSE);
	UpdateData(FALSE);
}

void CServerDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	OnCancel();
}

LRESULT CServerDlg::SockMsg(WPARAM wParam, LPARAM lParam)
{
	if (WSAGETSELECTERROR(lParam))
	{
		// Display the error and close the socket
		closesocket(wParam);
	}
	switch(WSAGETSELECTEVENT(lParam))
	{
	case FD_ACCEPT:
		{
			pSock[number_Socket].sockClient = accept(wParam,NULL,NULL);
			//GetDlgItem(IDC_LISTEN)->EnableWindow(FALSE);
			break;
		}
	case FD_READ:
		{
			int post = -1, dpos = -1;

			for(int i=0;i<number_Socket;i++)
			{
				if(pSock[i].sockClient==wParam)
				{
					if (i<number_Socket)
						post=i;
				}
			}
			CString temp;
			if(mRecv(wParam, temp) < 0)
				break;
			Split(temp,strResult);
			int flag =_ttoi(strResult[0]);
			
			char* mes1 = ConvertToChar(strResult[1]);
			char* mes2 = ConvertToChar(strResult[2]);
			switch(flag)
			{
				case 1://Login
					{
						int t = 0;
						if(number_Socket>0)
						{
							for(int i=0;i<number_Socket;i++)
							{
								if(strcmp(mes1,pSock[i].Name)==0)//Username existed
								{
									t=1;
									break;
								}
							}
						}
						if (checkLogin(mes1, mes2) != 1)
						{
							t = 1;
						}
						if(t==0)
						{
							strcpy(pSock[number_Socket].Name,mes1);
							Command=_T("1\r\n1\r\n");
							Command += pSock[number_Socket].Name;
							Command+=_T("\r\n");
							m_msgString+=strResult[1] + _T(" LogIn\r\n");
							
							OnlineClient += pSock[number_Socket].Name;//Update client in ONLINE CLIENT LIST
							OnlineClient += _T("\r\n");
							vtManagedClient.push_back(pSock[number_Socket].Name);
							number_Socket++;
							UpdateData(FALSE);
							for (int i = 0; i < number_Socket; i++)
							{
								mSend(pSock[i].sockClient, Command);
							}
						}
						else
						{
							Command = _T("1\r\n0\r\n");
							Command += _T("\r\n");
							mSend(wParam, Command);
						}
						Command = _T("5\r\n");
						Command += OnlineClient;
						for (int i = 0; i < number_Socket; i++)
						{
							mSend(pSock[i].sockClient, Command);
						}
						UpdateData(TRUE);
						break;
					}
				case 2://Sign up
					{
						int post = -1;
						for(int i=0;i<number_Socket;i++)
						{
							if(pSock[i].sockClient==wParam)
							{
								if (i<number_Socket)
									post=i;
							}
						}
						//Check if the name already exists or not
						if (checkSignUp(mes1) == 1)//name already exists
						{
							Command = _T("2\r\n0\r\n\r\n");
						}
						else
						{
							//add name into Account.txt
							updateData(mes1, mes2);
							Command = _T("2\r\n1\r\n\r\n");
							m_msgString += strResult[1] + _T(" SignUp\r\n");
							
						}
						mSend(wParam, Command);
						
						UpdateData(TRUE);
						break;
					}
				case 3:
				{
					m_msgString += strResult[1] + _T(": ");
					m_msgString += strResult[2] + _T("\r\n");
					for (int i = 0; i < number_Socket; i++)
					{
						mSend(pSock[i].sockClient, temp);
					}
					UpdateData(TRUE);
				}break;
				case 4:
					{	
						int post = -1;
						for(int i=0;i<number_Socket;i++)
						{	
							if(pSock[i].sockClient==wParam)
							{
								if (i<number_Socket)
									post=i;
							}
						}
						int i = checkUserName(pSock[post].Name);
						if (i == 1) {
							m_msgString += pSock[post].Name;
							m_msgString += _T(" LogOut\r\n");
							//Give notification: Client Logout
							Command = _T("3\r\n");
							Command += pSock[post].Name;
							Command += _T("\r\n");
							Command += _T(" LogOut\r\n");
							for (int i = 0; i < number_Socket; i++)
							{
								if (i != post)
									mSend(pSock[i].sockClient, Command);
							}

							//Remove Client from Online Client List
							CString temp(pSock[post].Name);
							int pos = OnlineClient.Find(temp, 0);
							OnlineClient.Delete(pos, temp.GetLength() + 2);
							vtManagedClient.erase(vtManagedClient.begin() + pos - 1);
							Command = _T("5\r\n");
							Command += OnlineClient;
							Command += _T("\r\n");
							for (int i = 0; i < number_Socket; i++)
							{
								if (i != pos)
									mSend(pSock[i].sockClient, Command);
							}
							closesocket(wParam);
							//Remove Socket from Arr
							for (int j = post; j < number_Socket - 1; j++)
							{
								pSock[j].sockClient = pSock[j + 1].sockClient;
								strcpy(pSock[j].Name, pSock[j + 1].Name);
							}
							number_Socket--;
							UpdateData(TRUE);
						}
					}break;
			}
		}break;
	case FD_CLOSE:
		{
			int post = -1;
			UpdateData();
			for(int i=0;i<number_Socket;i++)
			{
				if(pSock[i].sockClient==wParam)
				{
					if (i<number_Socket)
						post=i;
				}
			}
			//Name of Socket Log out
			int i = checkUserName(pSock[post].Name);
			if (i == 1) {
				m_msgString += pSock[post].Name;
				m_msgString += " LogOut\r\n";
				//Give notification: Client Logout
				Command = _T("3\r\n");
				Command += pSock[post].Name;
				Command += _T("\r\n");
				Command += _T(" LogOut\r\n");
				for (int i = 0; i < number_Socket; i++)
				{
					if (i != post)
						mSend(pSock[i].sockClient, Command);
				}
				//Remove Client from Online Client List
				CString temp(pSock[post].Name);
				int pos = OnlineClient.Find(temp, 0);
				OnlineClient.Delete(pos, temp.GetLength() + 2);
				Command = _T("5\r\n");
				Command += OnlineClient;
				Command += _T("\r\n");
				for (int i = 0; i < number_Socket; i++)
				{
					if (i != pos)
						mSend(pSock[i].sockClient, Command);
				}
				closesocket(wParam);
				for (int j = post; j < number_Socket - 1; j++)
				{
					pSock[j].sockClient = pSock[j + 1].sockClient;
					strcpy(pSock[j].Name, pSock[j + 1].Name);
				}
				number_Socket--;
				UpdateData(FALSE);
			}
			break;
		}
	}
	return 0;
}

int CServerDlg::checkLogin(char* Name, char* Pass)
{
	fstream file;
	char* namefile = "Account.txt";
	file.open(namefile);
	if (file.fail())
	{
		MessageBox(_T("Can't open file"), _T("ERROR"), MB_ICONERROR);
		return 0;
	}
	string name, pass;
	while (!file.eof())
	{
		file >> name >> pass;
		if ((strcmp(Name, name.c_str()) == 0) && (strcmp(Pass, pass.c_str()) == 0))
		{
			file.close();
			return 1;
		}
	}
	file.close();
	return 0;
}

int CServerDlg::checkUserName(char* Name)
{
	fstream file;
	char* namefile = "Account.txt";
	file.open(namefile);
	if (file.fail())
	{
		MessageBox(_T("Can't open file"), _T("ERROR"), MB_ICONERROR);
		return 0;
	}
	string name, pass;
	while (!file.eof())
	{
		file >> name >> pass;
		if (strcmp(Name, name.c_str()) == 0)
		{
			file.close();
			return 1;
		}
	}
	file.close();
	return 0;
}

int CServerDlg::checkSignUp(char* Name)
{
	fstream file;
	char* namefile = "Account.txt";
	file.open(namefile);
	if (file.fail())
	{
		MessageBox(_T("Can't open file"), _T("ERROR"), MB_ICONERROR);
		return 0;
	}
	string name, pass;
	while (!file.eof())
	{
		file >> name >> pass;
		if (strcmp(Name, name.c_str()) == 0)
		{
			file.close();
			return 1;
		}
	}
	file.close();
	return 0;
}

int CServerDlg::updateData(char* Name, char* Pass)
{
	fstream file;
	char* namefile = "Account.txt";
	file.open(namefile, ios::app);
	if (file.fail())
	{
		MessageBox(_T("Can't open file"),_T("ERROR"),MB_ICONERROR);
		return 0;
	}
	string name(Name);
	string pass(Pass);
	file << "\n" << name << " " << pass;
	file.close();
	return 1;
}

int CServerDlg::loadData()
{
	fstream file;
	char* namefile = "Account.txt";
	file.open(namefile);
	if (file.fail())
	{
		MessageBox(_T("Can't open file"), _T("ERROR"), MB_ICONERROR);
		return 0;
	}
	string name, pass;
	while (!file.eof())
	{
		file >> name >> pass;
		CString name(name.c_str());
		ManagedClient += name;
		ManagedClient += _T("\r\n");
	}
	UpdateData(FALSE);
	file.close();
	return 1;
}






void CServerDlg::OnEnChangeBoxchat()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}
