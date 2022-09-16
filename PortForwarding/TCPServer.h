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

	BOOL InitNetworkOfRecipient();									// 接收者的注册函数
	void OnAcceptOfRecipient(SOCKET CurSock);						// 接收者的连接请求函数
	void OnSendOfRecipient(char *buf, int len);						// 接收者的发送函数
	void OnReceiveOfRecipient(SOCKET CurSock);						// 接收者的接收函数

	BOOL InitNetworkOfSender();										// 发送者的注册函数
	void OnAcceptOfSender(SOCKET CurSock);							// 发送者的连接请求函数
	void OnSendOfSender(char *buf, int len);						// 发送者的发送函数
	void OnReceiveOfSender(SOCKET CurSock);							// 发送者的接收函数
	

	void OnClose(SOCKET CurSock);	
};