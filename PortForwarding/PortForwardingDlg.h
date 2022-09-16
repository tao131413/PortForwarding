
// PortForwardingDlg.h : ͷ�ļ�
//

#pragma once

// CPortForwardingDlg �Ի���
class CPortForwardingDlg : public CDialogEx
{
// ����
public:
	CPortForwardingDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_PORTFORWARDING_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedStart();
	afx_msg void OnBnClickedStop();
protected:
	afx_msg LRESULT OnSockeServerOfRecipient(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSockeServerOfSender(WPARAM wParam, LPARAM lParam);

	afx_msg LRESULT OnSockeClientOfRecipient(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSockeClientOfSender(WPARAM wParam, LPARAM lParam);
};
