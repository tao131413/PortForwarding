
// PortForwardingDlg.cpp : ʵ���ļ�
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


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CPortForwardingDlg �Ի���



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

// �����ߵķ����߳�
DWORD WINAPI RecipientThread(LPVOID pParam)
{
	char buf[2048] = { 0 };

	while (1)
	{
		// ����
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

// �����ߵķ����߳�
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

// CPortForwardingDlg ��Ϣ�������

void Log(char *data)
{
	USES_CONVERSION;

	unsigned char LogBuf[1024] = { 0 };
	SYSTEMTIME st;

	GetLocalTime(&st);
	sprintf((char *)LogBuf, "[%04d-%02d-%02d %02d:%02d:%02d:%03d] %s", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, data);

	LOG_WRITE->AddString((CString)LogBuf);
	LOG_WRITE->RedrawWindow();

	//����һ���Զ����� ��ʾ���һ��
	int count = LOG_WRITE->GetCount();
	LOG_WRITE->SetCurSel(count - 1);
}

BOOL CPortForwardingDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO:  �ڴ���Ӷ���ĳ�ʼ������

	WORD wVersionRequested;
	WSADATA wsaData;
	int ret = 0;

	wVersionRequested = MAKEWORD(1, 1);
	ret = WSAStartup(wVersionRequested, &wsaData);
	if (ret != 0)
	{
		Log("WSAStartup����");
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

	// �����߳�
	AfxBeginThread((AFX_THREADPROC)RecipientThread, NULL);
	AfxBeginThread((AFX_THREADPROC)SenderThread, NULL);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CPortForwardingDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CPortForwardingDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// ��������Ϣ����
afx_msg LRESULT CPortForwardingDlg::OnSockeServerOfRecipient(WPARAM wParam, LPARAM lParam)
{
	if (WSAGETASYNCERROR(lParam))
	{
		closesocket(wParam);
		return -1;
	}

	switch (WSAGETSELECTEVENT(lParam))
	{
	case FD_ACCEPT://ע���׽��֣��пͻ�������ʱ������
		TCPServerObj.OnAcceptOfRecipient(wParam);
		break;
	case FD_READ://��ȡ�׽�����Ϣ�����յ���Ϣʱ������
		TCPServerObj.OnReceiveOfRecipient(wParam);
		break;
	case FD_CLOSE://�ر��׽���
		Log("�����߷�����յ��ͻ��˶Ͽ�֪ͨ");
		TCPServerObj.OnClose(wParam);
		break;
	default:
		break;
	}
	return 0;
}

// ��������Ϣ����
afx_msg LRESULT CPortForwardingDlg::OnSockeServerOfSender(WPARAM wParam, LPARAM lParam)
{
	if (WSAGETASYNCERROR(lParam))
	{
		closesocket(wParam);
		return -1;
	}

	switch (WSAGETSELECTEVENT(lParam))
	{
	case FD_ACCEPT://ע���׽��֣��пͻ�������ʱ������
		TCPServerObj.OnAcceptOfSender(wParam);
		break;
	case FD_READ://��ȡ�׽�����Ϣ�����յ���Ϣʱ������
		TCPServerObj.OnReceiveOfSender(wParam);
		break;
	case FD_CLOSE://�ر��׽���
		Log("�����߷�����յ��ͻ��˶Ͽ�֪ͨ");
		TCPServerObj.OnClose(wParam);
		break;
	default:
		break;
	}
	return 0;
}

// ��������Ϣ����
afx_msg LRESULT CPortForwardingDlg::OnSockeClientOfRecipient(WPARAM wParam, LPARAM lParam)
{
	if (WSAGETASYNCERROR(lParam))
	{
		closesocket(wParam);
		return -1;
	}

	switch (WSAGETSELECTEVENT(lParam))
	{
	case FD_READ://��ȡ�׽�����Ϣ�����յ���Ϣʱ������
		TCPServerObj.OnReceiveOfRecipient(wParam);
		break;
	case FD_CLOSE://�ر��׽���
		Log("�����߷�����յ��ͻ��˶Ͽ�֪ͨ");
		TCPServerObj.OnClose(wParam);
		break;
	default:
		break;
	}
	return 0;
}

// ��������Ϣ����
afx_msg LRESULT CPortForwardingDlg::OnSockeClientOfSender(WPARAM wParam, LPARAM lParam)
{
	if (WSAGETASYNCERROR(lParam))
	{
		closesocket(wParam);
		return -1;
	}

	switch (WSAGETSELECTEVENT(lParam))
	{
	case FD_READ://��ȡ�׽�����Ϣ�����յ���Ϣʱ������
		TCPServerObj.OnReceiveOfSender(wParam);
		break;
	case FD_CLOSE://�ر��׽���
		Log("�����߷�����յ��ͻ��˶Ͽ�֪ͨ");
		TCPServerObj.OnClose(wParam);
		break;
	default:
		break;
	}
	return 0;
}

// ��ʼ��������
void CPortForwardingDlg::OnBnClickedStart()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CString strIP1, port1, strIP2, port2;

	// ��ȡ����IP
	GetDlgItem(IDC_IPADDRESS1)->GetWindowText(strIP1);
	GetDlgItem(IDC_IPADDRESS2)->GetWindowText(strIP2);

	// ��ȡ�˿�
	GetDlgItem(IDC_EDIT1)->GetWindowText(port1);
	GetDlgItem(IDC_EDIT2)->GetWindowText(port2);

	// ��ȡ����
	type1 = ((CComboBox*)GetDlgItem(IDC_COMBO1))->GetCurSel();
	type2 = ((CComboBox*)GetDlgItem(IDC_COMBO2))->GetCurSel();

	// ����
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

	// ����
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

// ����
void CPortForwardingDlg::OnBnClickedStop()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
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

