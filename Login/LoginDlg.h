
// LoginDlg.h : ͷ�ļ�
//

#pragma once
#include "afxcmn.h"


// CLoginDlg �Ի���
class CLoginDlg : public CDialogEx
{
// ����
public:
	CLoginDlg(CWnd* pParent = NULL);	// ��׼���캯��
	
// �Ի�������
	enum { IDD = IDD_LOGIN_DIALOG };

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

