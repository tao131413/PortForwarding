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


// �����ߵ�ע�ắ��
BOOL TCPClient::InitNetworkOfRecipient()
{
	//�������������׽���
	socketOfRecipient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (socketOfRecipient == INVALID_SOCKET)
	{
		Log("�����߿ͻ��˴���SOCKETʧ��");
		closesocket(socketOfRecipient);
		return FALSE;
	}

	//�󶨵�����һ���˿���
	sockaddr_in localaddr;
	localaddr.sin_family = AF_INET;
	localaddr.sin_port = htons(atoi(portOfRecipient.GetBuffer()));
	localaddr.sin_addr.s_addr = inet_addr(strIPOfRecipient.GetBuffer());
	if (connect(socketOfRecipient, (SOCKADDR *)&localaddr, sizeof(localaddr)) < 0)
	{
		Log("�����߿ͻ������ӷ����ʧ��");
		return FALSE;
	}

	//�첽������ģʽ��ע����������첽�¼�
	if (WSAAsyncSelect(socketOfRecipient, AfxGetMainWnd()->m_hWnd, WM_SOCKET_CLIENT_RECIPIENT, FD_CLOSE | FD_READ) == SOCKET_ERROR)
	{
		Log("������ע�������첽�¼�ʧ��");
		closesocket(socketOfRecipient);
		return FALSE;
	}

	Log("�����߿ͻ��˳ɹ����ӷ����");

	return TRUE;
}

// �����ߵķ��ͺ���
void TCPClient::OnSendOfRecipient(char *buf, int len)
{
	// �����ݷ���ʱ�������ݷ��ص����ӵ��豸
	int ret = 0;

	if ((ret = pLinkLoopRecvObj->read(buf, len)) > 0)
	{
		if (socketOfRecipient > 0)
			send(socketOfRecipient, buf, ret, 0);
	}
}

// �����ߵĽ��պ���
void TCPClient::OnReceiveOfRecipient(SOCKET CurSock)
{
	char buf[2048] = { 0 };
	int ret = 0;

	if ((ret = recv(CurSock, buf, sizeof(buf), 0)) > 0)
	{
		pLinkLoopSendObj->write(buf, ret);
	}
}

// �����ߵ�ע�ắ��
BOOL TCPClient::InitNetworkOfSender()
{
	//�������������׽���
	socketOfSender = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (socketOfSender == INVALID_SOCKET)
	{
		Log("�����߿ͻ��˴���SOCKETʧ��");
		closesocket(socketOfSender);
		return FALSE;
	}

	//�󶨵�����һ���˿���
	sockaddr_in localaddr;
	localaddr.sin_family = AF_INET;
	localaddr.sin_port = htons(atoi(portOfSender.GetBuffer()));
	localaddr.sin_addr.s_addr = inet_addr(strIPOfSender.GetBuffer());
	if (connect(socketOfSender, (SOCKADDR *)&localaddr, sizeof(localaddr)) < 0)
	{
		Log("�����߿ͻ������ӷ����ʧ��");
		return FALSE;
	}

	//�첽������ģʽ��ע����������첽�¼�
	if (WSAAsyncSelect(socketOfSender, AfxGetMainWnd()->m_hWnd, WM_SOCKET_CLIENT_SENDER, FD_CLOSE | FD_READ) == SOCKET_ERROR)
	{
		Log("������ע�������첽�¼�ʧ��");
		closesocket(socketOfSender);
		return FALSE;
	}

	Log("�����߿ͻ��˳ɹ����ӷ����");

	return TRUE;
}

// �����ߵķ��ͺ���
void TCPClient::OnSendOfSender(char *buf, int len)
{
	int ret = 0;

	if ((ret = pLinkLoopSendObj->read(buf, len)) > 0)
	{
		if (socketOfSender > 0)
			send(socketOfSender, buf, ret, 0);
	}
}

// �����ߵĽ��պ���
void TCPClient::OnReceiveOfSender(SOCKET CurSock)
{
	// �����ݷ���ʱ�������ݱ��浽��������
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
