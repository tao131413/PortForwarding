
// PortForwarding.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CPortForwardingApp: 
// �йش����ʵ�֣������ PortForwarding.cpp
//

class CPortForwardingApp : public CWinApp
{
public:
	CPortForwardingApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CPortForwardingApp theApp;