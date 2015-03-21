
// LoginDlg.cpp : 实现文件
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


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CLoginDlg 对话框

//全局数据
LoginNetConnect Connection[6];//网络连接数据

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


// CLoginDlg 消息处理程序

BOOL CLoginDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO:  在此添加额外的初始化代码

	//------------------------初始化列表框---------------------------------//
	LONG lStyle;
	lStyle = GetWindowLong(List.m_hWnd, GWL_STYLE);//获取当前窗口style
	lStyle &= ~LVS_TYPEMASK; //清除显示方式位
	lStyle |= LVS_REPORT; //设置style
	SetWindowLong(List.m_hWnd, GWL_STYLE, lStyle);//设置style

	DWORD dwStyle = List.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;//选中某行使整行高亮（只适用于report风格的listctrl）
	dwStyle |= LVS_EX_GRIDLINES;//网格线（只适用于report风格的listctrl）
	//dwStyle |= LVS_EX_CHECKBOXES;//item前生成checkbox控件
	List.SetExtendedStyle(dwStyle); //设置扩展风格
	List.InsertColumn(0, _T("#"), LVCFMT_LEFT, 80);
	List.InsertColumn(1, _T("连接状态"), LVCFMT_CENTER, 60);
	List.InsertColumn(2, _T("账户余额"), LVCFMT_CENTER, 70);
	List.InsertColumn(3, _T("已使用流量"), LVCFMT_CENTER, 80);
	List.InsertColumn(4, _T("剩余免费流量"), LVCFMT_CENTER, 100);
	List.InsertColumn(5, _T("剩余基础流量"), LVCFMT_CENTER, 100);
	List.InsertColumn(6, _T("超出基础流量"), LVCFMT_CENTER, 100);
	List.InsertColumn(7, _T("超流量费用"), LVCFMT_CENTER, 80);
	List.InsertColumn(8, _T("套餐类型"), LVCFMT_CENTER, 80);
	//List.InsertColumn(4, _T("进度"), LVCFMT_CENTER, 150);

	//初始化列表内容
	OnRefresh();
	std::sort(Connection, Connection + 6);
	Connection[0].BasicDataBalance;
	for (int i = 0; i <= 4; i++)//未来将会使用vector变长数组代替
		if (Connection[i].ConnectStatus == _T("用户已在本主机上登录，不需要重新登录"))
			Connection[i].OnBjfuLogin(Connection[i].Account, Connection[i].Password, (CStringA)"118.228.171.91", (CStringA)"disconnect");
	Connection[5].OnBjfuLogin(Connection[5].Account, Connection[5].Password, (CStringA)"118.228.171.91", (CStringA)"connect");
	OnRefresh();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CLoginDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CLoginDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CLoginDlg::OnRefresh()
{
	// 登陆网络
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

		if (Connection[i].ConnectStatus == _T("用户已在本主机上登录，不需要重新登录")
			||Connection[i].ConnectStatus==_T("联网成功"))
				TempStr = _T("√");
		else if (Connection[i].ConnectStatus == _T("对不起，您的用户状态不正确，您现在的状态是：[欠费]，请与管理员联系！"))
			TempStr = _T("×");
		else
			TempStr = _T("");
		List.SetItemText(tmp, 1, TempStr);

		TempStr.Format(_T("%.3lf"), Connection[i].AccountBalance);
		TempStr.AppendFormat(_T("元"));
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
		TempStr.AppendFormat(_T("元"));
		List.SetItemText(tmp, 7, TempStr);

		TempStr.Format(_T("%.0lf"), Connection[i].BasicDataTotal);
		TempStr.AppendFormat(_T("GB"));
		List.SetItemText(tmp, 8, TempStr);
	}

}



void CLoginDlg::OnAbout()
{
	// TODO:  在此添加控件通知处理程序代码
	CAboutDlg Dlg;
	Dlg.DoModal();
}
