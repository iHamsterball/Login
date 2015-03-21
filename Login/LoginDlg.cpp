
// LoginDlg.cpp : ʵ���ļ�
//


#include "stdafx.h"
#include "Login.h"
#include "LoginDlg.h"
#include "LoginNetConnect.h"
#include "afxdialogex.h"
#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
	EnableActiveAccessibility();
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CLoginDlg �Ի���

//ȫ������
LoginNetConnect Connection[6];//������������

CLoginDlg::CLoginDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CLoginDlg::IDD, pParent)
{
	EnableActiveAccessibility();
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CLoginDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, List);
}

BEGIN_MESSAGE_MAP(CLoginDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CLoginDlg::OnRefresh)
	ON_BN_CLICKED(IDC_BUTTON2, &CLoginDlg::OnAbout)
END_MESSAGE_MAP()


// CLoginDlg ��Ϣ�������

BOOL CLoginDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO:  �ڴ���Ӷ���ĳ�ʼ������

	//------------------------��ʼ���б��---------------------------------//
	LONG lStyle;
	lStyle = GetWindowLong(List.m_hWnd, GWL_STYLE);//��ȡ��ǰ����style
	lStyle &= ~LVS_TYPEMASK; //�����ʾ��ʽλ
	lStyle |= LVS_REPORT; //����style
	SetWindowLong(List.m_hWnd, GWL_STYLE, lStyle);//����style

	DWORD dwStyle = List.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;//ѡ��ĳ��ʹ���и�����ֻ������report����listctrl��
	dwStyle |= LVS_EX_GRIDLINES;//�����ߣ�ֻ������report����listctrl��
	//dwStyle |= LVS_EX_CHECKBOXES;//itemǰ����checkbox�ؼ�
	List.SetExtendedStyle(dwStyle); //������չ���
	List.InsertColumn(0, _T("#"), LVCFMT_LEFT, 80);
	List.InsertColumn(1, _T("����״̬"), LVCFMT_CENTER, 60);
	List.InsertColumn(2, _T("�˻����"), LVCFMT_CENTER, 70);
	List.InsertColumn(3, _T("��ʹ������"), LVCFMT_CENTER, 80);
	List.InsertColumn(4, _T("ʣ���������"), LVCFMT_CENTER, 100);
	List.InsertColumn(5, _T("ʣ���������"), LVCFMT_CENTER, 100);
	List.InsertColumn(6, _T("������������"), LVCFMT_CENTER, 100);
	List.InsertColumn(7, _T("����������"), LVCFMT_CENTER, 80);
	List.InsertColumn(8, _T("�ײ�����"), LVCFMT_CENTER, 80);
	//List.InsertColumn(4, _T("����"), LVCFMT_CENTER, 150);

	//��ʼ���б�����
	OnRefresh();
	std::sort(Connection, Connection + 6);
	Connection[0].BasicDataBalance;
	for (int i = 0; i <= 4; i++)//δ������ʹ��vector�䳤�������
		if (Connection[i].ConnectStatus == _T("�û����ڱ������ϵ�¼������Ҫ���µ�¼"))
			Connection[i].OnBjfuLogin(Connection[i].Account, Connection[i].Password, (CStringA)"118.228.171.91", (CStringA)"disconnect");
	Connection[5].OnBjfuLogin(Connection[5].Account, Connection[5].Password, (CStringA)"118.228.171.91", (CStringA)"connect");
	OnRefresh();

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CLoginDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CLoginDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CLoginDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CLoginDlg::OnRefresh()
{
	// ��½����
	CStringA username[6] = { "131114203", "131114205", "131114206", "131114207", "131114209", "131114215" };
	CStringA password[6] = { "5168079", "52222919", "41133018", "1124Cother", "cxzwlan", "bhx20140501" };
	CStringA ip, action;
	ip = _T("118.228.171.91");
	action = _T("admin");
	List.DeleteAllItems();
	for (int i = 5; i >= 0; i--)
	{
		int tmp = List.InsertItem(0, (CString)username[i]);
		Connection[i].OnBjfuLogin(username[i], password[i], ip, action);

		CString TempStr;

		if (Connection[i].ConnectStatus == _T("�û����ڱ������ϵ�¼������Ҫ���µ�¼")
			||Connection[i].ConnectStatus==_T("�����ɹ�"))
				TempStr = _T("��");
		else if (Connection[i].ConnectStatus == _T("�Բ��������û�״̬����ȷ�������ڵ�״̬�ǣ�[Ƿ��]���������Ա��ϵ��"))
			TempStr = _T("��");
		else
			TempStr = _T("");
		List.SetItemText(tmp, 1, TempStr);

		TempStr.Format(_T("%.3lf"), Connection[i].AccountBalance);
		TempStr.AppendFormat(_T("Ԫ"));
		List.SetItemText(tmp, 2, TempStr);
		TempStr.Format(_T("%.3lf"), Connection[i].DataUsage);
		TempStr.AppendFormat(_T("GB"));
		List.SetItemText(tmp, 3, TempStr);
		TempStr.Format(_T("%.3lf"), Connection[i].FreeDataBalance);
		TempStr.AppendFormat(_T("GB"));
		List.SetItemText(tmp, 4, TempStr);
		TempStr.Format(_T("%.3lf"), Connection[i].BasicDataBalance);
		TempStr.AppendFormat(_T("GB"));
		List.SetItemText(tmp, 5, TempStr);
		TempStr.Format(_T("%.3lf"), Connection[i].BasicDataExceed);
		TempStr.AppendFormat(_T("GB"));
		List.SetItemText(tmp, 6, TempStr);
		TempStr.Format(_T("%.3lf"), Connection[i].ExceedDataFee);
		TempStr.AppendFormat(_T("Ԫ"));
		List.SetItemText(tmp, 7, TempStr);

		TempStr.Format(_T("%.0lf"), Connection[i].BasicDataTotal);
		TempStr.AppendFormat(_T("GB"));
		List.SetItemText(tmp, 8, TempStr);
	}

}



void CLoginDlg::OnAbout()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CAboutDlg Dlg;
	Dlg.DoModal();
}
