
// LoginDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"


// CLoginDlg 对话框
class CLoginDlg : public CDialogEx
{
// 构造
public:
	CLoginDlg(CWnd* pParent = NULL);	// 标准构造函数
	
// 对话框数据
	enum { IDD = IDD_LOGIN_DIALOG };

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
	void OnCheckUpdate();
	afx_msg void OnRefresh();
	CListCtrl List;
	afx_msg void OnAbout();

	afx_msg void OnMenuAbout();
	afx_msg void OnMenuRefresh();
	afx_msg void OnMenuExit();
	afx_msg void OnOnlineSupport();
	afx_msg void OnMenuCheckUpdate();
	afx_msg void OnUpdateHistory();
};

