#pragma once

#define WM_SOCKET_RECIPIENT			(WM_USER + 1)
#define WM_SOCKET_SENDER			(WM_USER + 2)

class TCPServer
{
public:
	CString strIPOfRecipient, portOfRecipient;
	CString strIPOfSender, portOfSender;
	SOCKET socketOfRecipient = 0, socketOfSender = 0;
	SOCKET clientOfRecipient = 0, clientOfSender = 0;

	BOOL InitNetworkOfRecipient();									// �����ߵ�ע�ắ��
	void OnAcceptOfRecipient(SOCKET CurSock);						// �����ߵ�����������
	void OnSendOfRecipient(char *buf, int len);						// �����ߵķ��ͺ���
	void OnReceiveOfRecipient(SOCKET CurSock);						// �����ߵĽ��պ���

	BOOL InitNetworkOfSender();										// �����ߵ�ע�ắ��
	void OnAcceptOfSender(SOCKET CurSock);							// �����ߵ�����������
	void OnSendOfSender(char *buf, int len);						// �����ߵķ��ͺ���
	void OnReceiveOfSender(SOCKET CurSock);							// �����ߵĽ��պ���
	

	void OnClose(SOCKET CurSock);	
};