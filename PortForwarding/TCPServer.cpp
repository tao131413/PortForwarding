#include "stdafx.h"
#include <iostream>
#include <WINSOCK2.H>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <io.h>
#include <Mmsystem.h>
#include "LinkLoop.h"
#include "TCPServer.h"

extern void Log(char *data);
extern LinkLoop *pLinkLoopSendObj;
extern LinkLoop *pLinkLoopRecvObj;

// 接收者的注册函数
BOOL TCPServer::InitNetworkOfRecipient()
{
	//创建服务器端套接字
	socketOfRecipient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (socketOfRecipient == INVALID_SOCKET)
	{
		Log("接收者服务端创建SOCKET失败");
		closesocket(socketOfRecipient);
		return FALSE;
	}

	//绑定到本地一个端口上
	sockaddr_in localaddr;
	localaddr.sin_family = AF_INET;
	localaddr.sin_port = htons(atoi(portOfRecipient.GetBuffer()));
	localaddr.sin_addr.s_addr = inet_addr("0.0.0.0");
	if (bind(socketOfRecipient, (struct sockaddr*)&localaddr, sizeof(sockaddr)) == SOCKET_ERROR)
	{
		Log("服务端bind失败");
		closesocket(socketOfRecipient);
		return FALSE;
	}

	//将SeverSock设置为异步非阻塞模式，并为它注册各种网络异步事件，其中m_hWnd      
	//为应用程序的主对话框或主窗口的句柄
	if (WSAAsyncSelect(socketOfRecipient, AfxGetMainWnd()->m_hWnd, WM_SOCKET_RECIPIENT, FD_ACCEPT | FD_CLOSE | FD_READ | FD_WRITE) == SOCKET_ERROR)
	{
		Log("注册网络异步事件失败");
		closesocket(socketOfRecipient);
		return FALSE;
	}

	listen(socketOfRecipient, 5);

	Log("接收者服务端等待客户端连接");

	return TRUE;

}

// 接收者的连接请求函数
void TCPServer::OnAcceptOfRecipient(SOCKET CurSock)
{
	sockaddr_in clientaddr;
	CString str;
	int len = sizeof(clientaddr);
	HWND hWnd = AfxGetMainWnd()->GetSafeHwnd();

	clientOfRecipient = accept(CurSock, (sockaddr*)&clientaddr, &len);
	str = inet_ntoa(clientaddr.sin_addr);
	str.Append("连接到接收者服务端");
	Log(str.GetBuffer());

	//检查是否有数据到来
	WSAAsyncSelect(clientOfRecipient, hWnd, WM_SOCKET_RECIPIENT, FD_READ | FD_CLOSE);
}

// 接收者的发送函数
void TCPServer::OnSendOfRecipient(char *buf, int len)
{
	// 有数据返回时，把数据返回到连接的设备
	int ret = 0;

	if ((ret = pLinkLoopRecvObj->read(buf, len)) > 0)
	{
		if (clientOfRecipient > 0)
			send(clientOfRecipient, buf, ret, 0);
	}
}

// 接收者的接收函数
void TCPServer::OnReceiveOfRecipient(SOCKET CurSock)
{
	char buf[2048] = { 0 };
	int ret = 0;

	if ((ret = recv(CurSock, buf, sizeof(buf), 0)) > 0)
	{
		pLinkLoopSendObj->write(buf, ret);
	}
}

// 发送者的注册函数
BOOL TCPServer::InitNetworkOfSender()
{
	//创建服务器端套接字
	socketOfSender = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (socketOfSender == INVALID_SOCKET)
	{
		Log("发送者服务端创建SOCKET失败");
		closesocket(socketOfSender);
		return FALSE;
	}

	//绑定到本地一个端口上
	sockaddr_in localaddr;
	localaddr.sin_family = AF_INET;
	localaddr.sin_port = htons(atoi(portOfSender.GetBuffer()));
	localaddr.sin_addr.s_addr = inet_addr("0.0.0.0");
	if (bind(socketOfSender, (struct sockaddr*)&localaddr, sizeof(sockaddr)) == SOCKET_ERROR)
	{
		Log("服务端bind失败");
		closesocket(socketOfSender);
		return FALSE;
	}

	//将SeverSock设置为异步非阻塞模式，并为它注册各种网络异步事件，其中m_hWnd      
	//为应用程序的主对话框或主窗口的句柄
	if (WSAAsyncSelect(socketOfSender, AfxGetMainWnd()->m_hWnd, WM_SOCKET_SENDER, FD_ACCEPT | FD_CLOSE | FD_READ | FD_WRITE) == SOCKET_ERROR)
	{
		Log("注册网络异步事件失败");
		closesocket(socketOfSender);
		return FALSE;
	}

	listen(socketOfSender, 5);

	Log("发送者服务端等待客户端连接");

	return TRUE;
}

// 发送者的连接请求函数
void TCPServer::OnAcceptOfSender(SOCKET CurSock)
{
	sockaddr_in clientaddr;
	CString str;
	int len = sizeof(clientaddr);
	HWND hWnd = AfxGetMainWnd()->GetSafeHwnd();

	clientOfSender = accept(CurSock, (sockaddr*)&clientaddr, &len);
	str = inet_ntoa(clientaddr.sin_addr);
	str.Append("连接到发送者服务端");
	Log(str.GetBuffer());

	//检查是否有数据到来
	WSAAsyncSelect(clientOfSender, hWnd, WM_SOCKET_SENDER, FD_READ | FD_CLOSE);
}

// 发送者的发送函数
void TCPServer::OnSendOfSender(char *buf, int len)
{
	int ret = 0;

	if ((ret = pLinkLoopSendObj->read(buf, len)) > 0)
	{
		if (clientOfSender > 0)
			send(clientOfSender, buf, ret, 0);
	}
}

// 发送者的接收函数
void TCPServer::OnReceiveOfSender(SOCKET CurSock)
{
	// 有数据返回时，把数据保存到缓冲区中
	char buf[2048] = { 0 };
	int ret = 0;
	if ((ret = recv(CurSock, buf, sizeof(buf), 0)) > 0)
	{
		pLinkLoopRecvObj->write(buf, ret);
	}
}

void TCPServer::OnClose(SOCKET CurSock)
{
	closesocket(CurSock);
}

