#include "stdafx.h"
#include <iostream>
#include <WINSOCK2.H>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <io.h>
#include <Mmsystem.h>
#include "LinkLoop.h"
#include "TCPClient.h"

extern void Log(char *data);
extern int type1, type2;
extern LinkLoop *pLinkLoopSendObj;
extern LinkLoop *pLinkLoopRecvObj;


// 接收者的注册函数
BOOL TCPClient::InitNetworkOfRecipient()
{
	//创建服务器端套接字
	socketOfRecipient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (socketOfRecipient == INVALID_SOCKET)
	{
		Log("接收者客户端创建SOCKET失败");
		closesocket(socketOfRecipient);
		return FALSE;
	}

	//绑定到本地一个端口上
	sockaddr_in localaddr;
	localaddr.sin_family = AF_INET;
	localaddr.sin_port = htons(atoi(portOfRecipient.GetBuffer()));
	localaddr.sin_addr.s_addr = inet_addr(strIPOfRecipient.GetBuffer());
	if (connect(socketOfRecipient, (SOCKADDR *)&localaddr, sizeof(localaddr)) < 0)
	{
		Log("接收者客户端连接服务端失败");
		return FALSE;
	}

	//异步非阻塞模式，注册各种网络异步事件
	if (WSAAsyncSelect(socketOfRecipient, AfxGetMainWnd()->m_hWnd, WM_SOCKET_CLIENT_RECIPIENT, FD_CLOSE | FD_READ) == SOCKET_ERROR)
	{
		Log("接收者注册网络异步事件失败");
		closesocket(socketOfRecipient);
		return FALSE;
	}

	Log("接收者客户端成功连接服务端");

	return TRUE;
}

// 接收者的发送函数
void TCPClient::OnSendOfRecipient(char *buf, int len)
{
	// 有数据返回时，把数据返回到连接的设备
	int ret = 0;

	if ((ret = pLinkLoopRecvObj->read(buf, len)) > 0)
	{
		if (socketOfRecipient > 0)
			send(socketOfRecipient, buf, ret, 0);
	}
}

// 接收者的接收函数
void TCPClient::OnReceiveOfRecipient(SOCKET CurSock)
{
	char buf[2048] = { 0 };
	int ret = 0;

	if ((ret = recv(CurSock, buf, sizeof(buf), 0)) > 0)
	{
		pLinkLoopSendObj->write(buf, ret);
	}
}

// 发送者的注册函数
BOOL TCPClient::InitNetworkOfSender()
{
	//创建服务器端套接字
	socketOfSender = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (socketOfSender == INVALID_SOCKET)
	{
		Log("发送者客户端创建SOCKET失败");
		closesocket(socketOfSender);
		return FALSE;
	}

	//绑定到本地一个端口上
	sockaddr_in localaddr;
	localaddr.sin_family = AF_INET;
	localaddr.sin_port = htons(atoi(portOfSender.GetBuffer()));
	localaddr.sin_addr.s_addr = inet_addr(strIPOfSender.GetBuffer());
	if (connect(socketOfSender, (SOCKADDR *)&localaddr, sizeof(localaddr)) < 0)
	{
		Log("发送者客户端连接服务端失败");
		return FALSE;
	}

	//异步非阻塞模式，注册各种网络异步事件
	if (WSAAsyncSelect(socketOfSender, AfxGetMainWnd()->m_hWnd, WM_SOCKET_CLIENT_SENDER, FD_CLOSE | FD_READ) == SOCKET_ERROR)
	{
		Log("发送者注册网络异步事件失败");
		closesocket(socketOfSender);
		return FALSE;
	}

	Log("发送者客户端成功连接服务端");

	return TRUE;
}

// 发送者的发送函数
void TCPClient::OnSendOfSender(char *buf, int len)
{
	int ret = 0;

	if ((ret = pLinkLoopSendObj->read(buf, len)) > 0)
	{
		if (socketOfSender > 0)
			send(socketOfSender, buf, ret, 0);
	}
}

// 发送者的接收函数
void TCPClient::OnReceiveOfSender(SOCKET CurSock)
{
	// 有数据返回时，把数据保存到缓冲区中
	char buf[2048] = { 0 };
	int ret = 0;
	if ((ret = recv(CurSock, buf, sizeof(buf), 0)) > 0)
	{
		pLinkLoopRecvObj->write(buf, ret);
	}
}

void TCPClient::OnClose(SOCKET CurSock)
{
	closesocket(CurSock);
}
