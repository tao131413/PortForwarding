#pragma once

#define WM_SOCKET_CLIENT_RECIPIENT			(WM_USER + 3)
#define WM_SOCKET_CLIENT_SENDER				(WM_USER + 4)

class TCPClient
{
public:
	CString strIPOfRecipient, portOfRecipient;
	CString strIPOfSender, portOfSender;
	SOCKET socketOfRecipient = 0, socketOfSender = 0;

	BOOL InitNetworkOfRecipient();									// �����ߵ�ע�ắ��
	void OnSendOfRecipient(char *buf, int len);						// �����ߵķ��ͺ���
	void OnReceiveOfRecipient(SOCKET CurSock);						// �����ߵĽ��պ���

	BOOL InitNetworkOfSender();										// �����ߵ�ע�ắ��
	void OnSendOfSender(char *buf, int len);						// �����ߵķ��ͺ���
	void OnReceiveOfSender(SOCKET CurSock);							// �����ߵĽ��պ���


	void OnClose(SOCKET CurSock);
};