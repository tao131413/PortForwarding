
// PortForwardingDlg.h : 头文件
//

#pragma once

// CPortForwardingDlg 对话框
class CPortForwardingDlg : public CDialogEx
{
// 构造
public:
	CPortForwardingDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_PORTFORWARDING_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
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
