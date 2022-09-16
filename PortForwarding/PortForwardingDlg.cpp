
// PortForwardingDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "PortForwarding.h"
#include "PortForwardingDlg.h"
#include "afxdialogex.h"
#include "LinkLoop.h"
#include "TCPServer.h"
#include "TCPClient.h"
#include <map>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CListBox *LOG_WRITE;
CComboBox *ConnType1;
CComboBox *ConnType2;

int type1, type2;

LinkLoop LinkLoopSendObj, LinkLoopRecvObj;
LinkLoop *pLinkLoopSendObj = &LinkLoopSendObj;
LinkLoop *pLinkLoopRecvObj = &LinkLoopRecvObj;

char LinkLoopSendBuf[1024 * 30] = { 0 };
char LinkLoopRecvBuf[1024 * 30] = { 0 };

TCPClient TCPClientObj;
TCPServer TCPServerObj;


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CPortForwardingDlg 对话框



CPortForwardingDlg::CPortForwardingDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPortForwardingDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPortForwardingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CPortForwardingDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_START, &CPortForwardingDlg::OnBnClickedStart)
	ON_BN_CLICKED(IDC_STOP, &CPortForwardingDlg::OnBnClickedStop)
	ON_MESSAGE(WM_SOCKET_RECIPIENT, &CPortForwardingDlg::OnSockeServerOfRecipient)
	ON_MESSAGE(WM_SOCKET_SENDER, &CPortForwardingDlg::OnSockeServerOfSender)
	ON_MESSAGE(WM_SOCKET_CLIENT_RECIPIENT, &CPortForwardingDlg::OnSockeClientOfRecipient)
	ON_MESSAGE(WM_SOCKET_CLIENT_SENDER, &CPortForwardingDlg::OnSockeClientOfSender)
END_MESSAGE_MAP()

// 接收者的发送线程
DWORD WINAPI RecipientThread(LPVOID pParam)
{
	char buf[2048] = { 0 };

	while (1)
	{
		// 接收
		if (type1 == 0)
		{
			// tcp client
			TCPClientObj.OnSendOfRecipient(buf, sizeof(buf));
		}
		else if (type1 == 1)
		{
			// tcp server
			TCPServerObj.OnSendOfRecipient(buf, sizeof(buf));
		}
		else
		{
			// udp

		}

		Sleep(100);
	}
}

// 发送者的发送线程
DWORD WINAPI SenderThread(LPVOID pParam)
{
	char buf[2048] = { 0 };

	while (1)
	{
		if (type2 == 0)
		{
			// tcp client
			TCPClientObj.OnSendOfSender(buf, sizeof(buf));
		}
		else if (type2 == 1)
		{
			// tcp server
			TCPServerObj.OnSendOfSender(buf, sizeof(buf));
		}
		else
		{
			// udp

		}

		Sleep(100);
	}
}

// CPortForwardingDlg 消息处理程序

void Log(char *data)
{
	USES_CONVERSION;

	unsigned char LogBuf[1024] = { 0 };
	SYSTEMTIME st;

	GetLocalTime(&st);
	sprintf((char *)LogBuf, "[%04d-%02d-%02d %02d:%02d:%02d:%03d] %s", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, data);

	LOG_WRITE->AddString((CString)LogBuf);
	LOG_WRITE->RedrawWindow();

	//方法一：自动滚动 显示最后一行
	int count = LOG_WRITE->GetCount();
	LOG_WRITE->SetCurSel(count - 1);
}

BOOL CPortForwardingDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO:  在此添加额外的初始化代码

	WORD wVersionRequested;
	WSADATA wsaData;
	int ret = 0;

	wVersionRequested = MAKEWORD(1, 1);
	ret = WSAStartup(wVersionRequested, &wsaData);
	if (ret != 0)
	{
		Log("WSAStartup错误");
	}

	LOG_WRITE = (CListBox*)GetDlgItem(IDC_LOG);
	ConnType1 = (CComboBox *)GetDlgItem(IDC_COMBO1);
	ConnType2 = (CComboBox *)GetDlgItem(IDC_COMBO2);
	ConnType1->AddString("TCPServer");
	ConnType1->AddString("TCPClient");
	ConnType1->AddString("UDP");
	((CComboBox*)GetDlgItem(IDC_COMBO1))->SetCurSel(0);
	ConnType2->AddString("TCPServer");
	ConnType2->AddString("TCPClient");
	ConnType2->AddString("UDP");
	((CComboBox*)GetDlgItem(IDC_COMBO2))->SetCurSel(0);

	pLinkLoopSendObj->create(LinkLoopSendBuf, sizeof(LinkLoopSendBuf));
	pLinkLoopRecvObj->create(LinkLoopRecvBuf, sizeof(LinkLoopRecvBuf));

	// 创建线程
	AfxBeginThread((AFX_THREADPROC)RecipientThread, NULL);
	AfxBeginThread((AFX_THREADPROC)SenderThread, NULL);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CPortForwardingDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CPortForwardingDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CPortForwardingDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// 接收者消息函数
afx_msg LRESULT CPortForwardingDlg::OnSockeServerOfRecipient(WPARAM wParam, LPARAM lParam)
{
	if (WSAGETASYNCERROR(lParam))
	{
		closesocket(wParam);
		return -1;
	}

	switch (WSAGETSELECTEVENT(lParam))
	{
	case FD_ACCEPT://注册套接字（有客户机连接时出发）
		TCPServerObj.OnAcceptOfRecipient(wParam);
		break;
	case FD_READ://读取套接字信息（接收到消息时触发）
		TCPServerObj.OnReceiveOfRecipient(wParam);
		break;
	case FD_CLOSE://关闭套接字
		Log("接收者服务端收到客户端断开通知");
		TCPServerObj.OnClose(wParam);
		break;
	default:
		break;
	}
	return 0;
}

// 发送者消息函数
afx_msg LRESULT CPortForwardingDlg::OnSockeServerOfSender(WPARAM wParam, LPARAM lParam)
{
	if (WSAGETASYNCERROR(lParam))
	{
		closesocket(wParam);
		return -1;
	}

	switch (WSAGETSELECTEVENT(lParam))
	{
	case FD_ACCEPT://注册套接字（有客户机连接时出发）
		TCPServerObj.OnAcceptOfSender(wParam);
		break;
	case FD_READ://读取套接字信息（接收到消息时触发）
		TCPServerObj.OnReceiveOfSender(wParam);
		break;
	case FD_CLOSE://关闭套接字
		Log("发送者服务端收到客户端断开通知");
		TCPServerObj.OnClose(wParam);
		break;
	default:
		break;
	}
	return 0;
}

// 接收者消息函数
afx_msg LRESULT CPortForwardingDlg::OnSockeClientOfRecipient(WPARAM wParam, LPARAM lParam)
{
	if (WSAGETASYNCERROR(lParam))
	{
		closesocket(wParam);
		return -1;
	}

	switch (WSAGETSELECTEVENT(lParam))
	{
	case FD_READ://读取套接字信息（接收到消息时触发）
		TCPServerObj.OnReceiveOfRecipient(wParam);
		break;
	case FD_CLOSE://关闭套接字
		Log("接收者服务端收到客户端断开通知");
		TCPServerObj.OnClose(wParam);
		break;
	default:
		break;
	}
	return 0;
}

// 发送者消息函数
afx_msg LRESULT CPortForwardingDlg::OnSockeClientOfSender(WPARAM wParam, LPARAM lParam)
{
	if (WSAGETASYNCERROR(lParam))
	{
		closesocket(wParam);
		return -1;
	}

	switch (WSAGETSELECTEVENT(lParam))
	{
	case FD_READ://读取套接字信息（接收到消息时触发）
		TCPServerObj.OnReceiveOfSender(wParam);
		break;
	case FD_CLOSE://关闭套接字
		Log("发送者服务端收到客户端断开通知");
		TCPServerObj.OnClose(wParam);
		break;
	default:
		break;
	}
	return 0;
}

// 开始建立连接
void CPortForwardingDlg::OnBnClickedStart()
{
	// TODO:  在此添加控件通知处理程序代码
	CString strIP1, port1, strIP2, port2;

	// 获取接收IP
	GetDlgItem(IDC_IPADDRESS1)->GetWindowText(strIP1);
	GetDlgItem(IDC_IPADDRESS2)->GetWindowText(strIP2);

	// 获取端口
	GetDlgItem(IDC_EDIT1)->GetWindowText(port1);
	GetDlgItem(IDC_EDIT2)->GetWindowText(port2);

	// 获取类型
	type1 = ((CComboBox*)GetDlgItem(IDC_COMBO1))->GetCurSel();
	type2 = ((CComboBox*)GetDlgItem(IDC_COMBO2))->GetCurSel();

	// 接收
	if (type1 == 0)
	{
		// tcp client
		TCPClientObj.strIPOfRecipient = strIP1;
		TCPClientObj.portOfRecipient = port1;
		TCPClientObj.InitNetworkOfRecipient();
	}
	else if (type1 == 1)
	{
		// tcp server
		TCPServerObj.strIPOfRecipient = strIP1;
		TCPServerObj.portOfRecipient = port1;
		TCPServerObj.InitNetworkOfRecipient();
	}
	else
	{
		// udp

	}

	// 发送
	if (type2 == 0)
	{
		// tcp client
		TCPClientObj.strIPOfSender = strIP2;
		TCPClientObj.portOfSender = port2;
		TCPClientObj.InitNetworkOfSender();
	}
	else if (type2 == 1)
	{
		// tcp server
		TCPServerObj.strIPOfSender = strIP2;
		TCPServerObj.portOfSender = port2;
		TCPServerObj.InitNetworkOfSender();
	}
	else
	{
		// udp

	}

}

// 结束
void CPortForwardingDlg::OnBnClickedStop()
{
	// TODO:  在此添加控件通知处理程序代码
	if (TCPClientObj.socketOfRecipient != 0)
	{
		closesocket(TCPClientObj.socketOfRecipient);
		TCPClientObj.socketOfRecipient = 0;
	}

	if (TCPClientObj.socketOfSender != 0)
	{
		closesocket(TCPClientObj.socketOfSender);
		TCPClientObj.socketOfSender = 0;
	}

	if (TCPServerObj.socketOfSender != 0)
	{
		closesocket(TCPServerObj.socketOfSender);
		TCPServerObj.socketOfSender = 0;
	}

	if (TCPServerObj.socketOfRecipient != 0)
	{
		closesocket(TCPServerObj.socketOfRecipient);
		TCPServerObj.socketOfRecipient = 0;
	}
}

