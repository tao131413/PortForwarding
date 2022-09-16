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

// �����ߵ�ע�ắ��
BOOL TCPServer::InitNetworkOfRecipient()
{
	//�������������׽���
	socketOfRecipient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (socketOfRecipient == INVALID_SOCKET)
	{
		Log("�����߷���˴���SOCKETʧ��");
		closesocket(socketOfRecipient);
		return FALSE;
	}

	//�󶨵�����һ���˿���
	sockaddr_in localaddr;
	localaddr.sin_family = AF_INET;
	localaddr.sin_port = htons(atoi(portOfRecipient.GetBuffer()));
	localaddr.sin_addr.s_addr = inet_addr("0.0.0.0");
	if (bind(socketOfRecipient, (struct sockaddr*)&localaddr, sizeof(sockaddr)) == SOCKET_ERROR)
	{
		Log("�����bindʧ��");
		closesocket(socketOfRecipient);
		return FALSE;
	}

	//��SeverSock����Ϊ�첽������ģʽ����Ϊ��ע����������첽�¼�������m_hWnd      
	//ΪӦ�ó�������Ի���������ڵľ��
	if (WSAAsyncSelect(socketOfRecipient, AfxGetMainWnd()->m_hWnd, WM_SOCKET_RECIPIENT, FD_ACCEPT | FD_CLOSE | FD_READ | FD_WRITE) == SOCKET_ERROR)
	{
		Log("ע�������첽�¼�ʧ��");
		closesocket(socketOfRecipient);
		return FALSE;
	}

	listen(socketOfRecipient, 5);

	Log("�����߷���˵ȴ��ͻ�������");

	return TRUE;

}

// �����ߵ�����������
void TCPServer::OnAcceptOfRecipient(SOCKET CurSock)
{
	sockaddr_in clientaddr;
	CString str;
	int len = sizeof(clientaddr);
	HWND hWnd = AfxGetMainWnd()->GetSafeHwnd();

	clientOfRecipient = accept(CurSock, (sockaddr*)&clientaddr, &len);
	str = inet_ntoa(clientaddr.sin_addr);
	str.Append("���ӵ������߷����");
	Log(str.GetBuffer());

	//����Ƿ������ݵ���
	WSAAsyncSelect(clientOfRecipient, hWnd, WM_SOCKET_RECIPIENT, FD_READ | FD_CLOSE);
}

// �����ߵķ��ͺ���
void TCPServer::OnSendOfRecipient(char *buf, int len)
{
	// �����ݷ���ʱ�������ݷ��ص����ӵ��豸
	int ret = 0;

	if ((ret = pLinkLoopRecvObj->read(buf, len)) > 0)
	{
		if (clientOfRecipient > 0)
			send(clientOfRecipient, buf, ret, 0);
	}
}

// �����ߵĽ��պ���
void TCPServer::OnReceiveOfRecipient(SOCKET CurSock)
{
	char buf[2048] = { 0 };
	int ret = 0;

	if ((ret = recv(CurSock, buf, sizeof(buf), 0)) > 0)
	{
		pLinkLoopSendObj->write(buf, ret);
	}
}

// �����ߵ�ע�ắ��
BOOL TCPServer::InitNetworkOfSender()
{
	//�������������׽���
	socketOfSender = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (socketOfSender == INVALID_SOCKET)
	{
		Log("�����߷���˴���SOCKETʧ��");
		closesocket(socketOfSender);
		return FALSE;
	}

	//�󶨵�����һ���˿���
	sockaddr_in localaddr;
	localaddr.sin_family = AF_INET;
	localaddr.sin_port = htons(atoi(portOfSender.GetBuffer()));
	localaddr.sin_addr.s_addr = inet_addr("0.0.0.0");
	if (bind(socketOfSender, (struct sockaddr*)&localaddr, sizeof(sockaddr)) == SOCKET_ERROR)
	{
		Log("�����bindʧ��");
		closesocket(socketOfSender);
		return FALSE;
	}

	//��SeverSock����Ϊ�첽������ģʽ����Ϊ��ע����������첽�¼�������m_hWnd      
	//ΪӦ�ó�������Ի���������ڵľ��
	if (WSAAsyncSelect(socketOfSender, AfxGetMainWnd()->m_hWnd, WM_SOCKET_SENDER, FD_ACCEPT | FD_CLOSE | FD_READ | FD_WRITE) == SOCKET_ERROR)
	{
		Log("ע�������첽�¼�ʧ��");
		closesocket(socketOfSender);
		return FALSE;
	}

	listen(socketOfSender, 5);

	Log("�����߷���˵ȴ��ͻ�������");

	return TRUE;
}

// �����ߵ�����������
void TCPServer::OnAcceptOfSender(SOCKET CurSock)
{
	sockaddr_in clientaddr;
	CString str;
	int len = sizeof(clientaddr);
	HWND hWnd = AfxGetMainWnd()->GetSafeHwnd();

	clientOfSender = accept(CurSock, (sockaddr*)&clientaddr, &len);
	str = inet_ntoa(clientaddr.sin_addr);
	str.Append("���ӵ������߷����");
	Log(str.GetBuffer());

	//����Ƿ������ݵ���
	WSAAsyncSelect(clientOfSender, hWnd, WM_SOCKET_SENDER, FD_READ | FD_CLOSE);
}

// �����ߵķ��ͺ���
void TCPServer::OnSendOfSender(char *buf, int len)
{
	int ret = 0;

	if ((ret = pLinkLoopSendObj->read(buf, len)) > 0)
	{
		if (clientOfSender > 0)
			send(clientOfSender, buf, ret, 0);
	}
}

// �����ߵĽ��պ���
void TCPServer::OnReceiveOfSender(SOCKET CurSock)
{
	// �����ݷ���ʱ�������ݱ��浽��������
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

