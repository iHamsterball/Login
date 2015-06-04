
// LoginDlg.cpp : 实现文件
//


#include "stdafx.h"
#include "Login.h"
#include "LoginDlg.h"
#include "LoginNetConnect.h"
#include "afxdialogex.h"
#include <algorithm>
#pragma comment(lib, "version.lib")
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
public:
	virtual BOOL OnInitDialog();
	CString GetAppVersion(WCHAR* AppName);
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
	EnableActiveAccessibility();
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}
CString CAboutDlg::GetAppVersion(WCHAR* AppName)
{
	CString   AppVersion;

	DWORD   RessourceVersionInfoSize;
	DWORD   JustAJunkVariabel;
	WCHAR*   VersionInfoPtr;
	struct   LANGANDCODEPAGE
	{
		WORD   wLanguage;
		WORD   wCodePage;
	}   *TranslationPtr;
	WCHAR*     InformationPtr;
	UINT      VersionInfoSize;
	WCHAR     VersionValue[255];

	RessourceVersionInfoSize = GetFileVersionInfoSize(AppName, &JustAJunkVariabel);
	if (0 != RessourceVersionInfoSize)
	{
		VersionInfoPtr = new WCHAR[RessourceVersionInfoSize];
		if (!GetFileVersionInfo(AppName, 0, RessourceVersionInfoSize, VersionInfoPtr))
		{
			//ErrorExit((LPTSTR)L"GetFileVersionInfo");
			delete[]   VersionInfoPtr;
			return NULL;
		}

		if (!VerQueryValue(VersionInfoPtr, L"VarFileInfo\\Translation", (LPVOID*)&TranslationPtr, &VersionInfoSize))
		{
			//ErrorExit((LPTSTR)L"VerQueryValue");
			delete[]   VersionInfoPtr;
			return NULL;
		}

		// retrieve product version
		wsprintf(VersionValue, L"\\StringFileInfo\\%04x%04x\\ProductVersion", TranslationPtr[0].wLanguage, TranslationPtr[0].wCodePage);

		if (!VerQueryValue(VersionInfoPtr, VersionValue, (LPVOID*)&InformationPtr, &VersionInfoSize))
		{
			//ErrorExit((LPTSTR)L"VerQueryValue");
			delete[]   VersionInfoPtr;
			return NULL;
		}
		if (wcslen(InformationPtr)> 0)   //Not   Null 
		{
			AppVersion = CString(InformationPtr);
		}

		delete[]   VersionInfoPtr;
	}
	return   AppVersion;
}
BOOL CAboutDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CString ver = GetAppVersion(L"Login.exe");
	if (ver.IsEmpty()) return FALSE;

	int pos = ver.ReverseFind('.');
	CString mainVer = ver.Left(pos);
	CString build = ver.Right(ver.GetLength() - pos - 1);
	GetDlgItem(IDC_STATIC_VER)->SetWindowText(mainVer);
	GetDlgItem(IDC_STATIC_BUILD)->SetWindowText(build);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}
BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CLoginDlg 对话框

//全局数据
//LoginNetConnect Connection[6];//网络连接数据
std::vector<LoginNetConnect> vec;
std::vector<LoginNetConnect>::iterator cur;

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
	ON_COMMAND(ID_32774, &CLoginDlg::OnMenuAbout)
	ON_COMMAND(ID_32771, &CLoginDlg::OnMenuRefresh)
	ON_COMMAND(ID_32772, &CLoginDlg::OnMenuExit)
	ON_COMMAND(ID_32775, &CLoginDlg::OnOnlineSupport)
	ON_COMMAND(ID_32776, &CLoginDlg::OnMenuCheckUpdate)
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
	List.InsertColumn(3, _T("剩余免费流量"), LVCFMT_CENTER, 100);
	List.InsertColumn(4, _T("剩余基础流量"), LVCFMT_CENTER, 100);
	List.InsertColumn(5, _T("超出基础流量"), LVCFMT_CENTER, 100);
	List.InsertColumn(6, _T("超流量费用"), LVCFMT_CENTER, 80);
	List.InsertColumn(7, _T("已使用流量"), LVCFMT_CENTER, 80);
	List.InsertColumn(8, _T("套餐类型"), LVCFMT_CENTER, 80);
	//List.InsertColumn(4, _T("进度"), LVCFMT_CENTER, 150);

	//读取账号信息
	wchar_t buffer[2000];
	GetPrivateProfileSection(_T("info"), buffer, 2000, _T("./Config.ini"));
	//MessageBox(buffer, _T("info"), 0);
	CString username, password;
	LoginNetConnect account;
	int p;
	p = 0;
	int state = 1;//reading username
	while (true)
	{
		if (state == 1)
			account.Account.AppendChar(buffer[p]);
		else if (state == 2)
			account.Password.AppendChar(buffer[p]);
		else
			break;

		p++;
		if (buffer[p] == '=')
		{
			state = 2;
			p++;
		}
		else if (buffer[p] == '\0')
		{
			if (buffer[p + 1] == '\0')
			{
				state = 0;
				vec.push_back(account);
			}
			else
			{
				p++;
				state = 1;
				vec.push_back(account);
				account.Empty();
			}
		}
	}

	//初始化列表内容
	OnRefresh();
	std::sort(vec.begin(), vec.end());
	for (cur = vec.begin(); cur != vec.end();cur++)//未来将会使用vector变长数组代替
		if ((*cur).ConnectStatus == _T("用户已在本主机上登录，不需要重新登录")
			|| (*cur).ConnectStatus == _T("联网成功"))
		{
			(*cur).OnBjfuLogin((*cur).Account, (*cur).Password, (CStringA)"118.228.171.91", (CStringA)"disconnect");
			break;
		}
	//需要考虑网络连接数已满的情况
	for (cur = vec.begin(); cur != vec.end(); cur++)
	{
		if ((*cur).BasicDataBalance > 0)
			(*cur).OnBjfuLogin((*cur).Account, (*cur).Password, (CStringA)"118.228.171.91", (CStringA)"connect");
		if (!((*cur).ConnectStatus == _T("用户已在本主机上登录，不需要重新登录")
			|| (*cur).ConnectStatus == _T("联网成功")))
			continue;
		else
			break;
	}
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
	CStringA ip;//在当前版本中，不需要手动指定IP地址，程序会自动获取当前连接网络的IP地址
	CStringA action;
	action = _T("admin");
	List.DeleteAllItems();
	for (cur = vec.begin(); cur != vec.end(); cur++)
	{
		int tmp = List.InsertItem(0, (CString)(*cur).Account);
		(*cur).OnBjfuLogin((*cur).Account, (*cur).Password, ip, action);

		CString TempStr;

		if ((*cur).ConnectStatus == _T("用户已在本主机上登录，不需要重新登录")
			|| (*cur).ConnectStatus == _T("联网成功"))
			TempStr = _T("√");
		else if ((*cur).ConnectStatus == _T("对不起，您的用户状态不正确，您现在的状态是：[欠费]，请与管理员联系！"))
			TempStr = _T("×");
		else if ((*cur).ConnectStatus == _T("密码错误"))
			TempStr = _T("Error 1");
		else
			TempStr = _T("");
		List.SetItemText(tmp, 1, TempStr);

		if (TempStr != _T("Error 1"))
		{
			TempStr.Format(_T("%.3lf"), (*cur).AccountBalance);
			TempStr.AppendFormat(_T("元"));
			List.SetItemText(tmp, 2, TempStr);
			TempStr.Format(_T("%.3lf"), (*cur).FreeDataBalance);
			TempStr.AppendFormat(_T("GB"));
			List.SetItemText(tmp, 3, TempStr);
			TempStr.Format(_T("%.3lf"), (*cur).BasicDataBalance);
			TempStr.AppendFormat(_T("GB"));
			List.SetItemText(tmp, 4, TempStr);
			TempStr.Format(_T("%.3lf"), (*cur).BasicDataExceed);
			TempStr.AppendFormat(_T("GB"));
			List.SetItemText(tmp, 5, TempStr);
			TempStr.Format(_T("%.3lf"), (*cur).ExceedDataFee);
			TempStr.AppendFormat(_T("元"));
			List.SetItemText(tmp, 6, TempStr);
			TempStr.Format(_T("%.3lf"), (*cur).DataUsage);
			TempStr.AppendFormat(_T("GB"));
			List.SetItemText(tmp, 7, TempStr);

			TempStr.Format(_T("%.0lf"), (*cur).BasicDataTotal);
			TempStr.AppendFormat(_T("GB"));
			List.SetItemText(tmp, 8, TempStr);
		}
		else
		{
			TempStr = _T("-");
			for (int i = 2; i <= 8;i++)
				List.SetItemText(tmp, i, TempStr);
		}
	}

}


void CLoginDlg::OnCheckUpdate()
{
	//获取当前文件的版本信息
	CAboutDlg Dlg;
	CString ver = Dlg.GetAppVersion(_T("Login.exe"));
	if (ver.IsEmpty()) return;

	int pos = ver.ReverseFind('.');
	CString mainVer = ver.Left(pos);
	CString build = ver.Right(ver.GetLength() - pos - 1);

	//获取最新更新信息
	CString cache = _T(""); 
	CInternetSession  session;
	CHttpFile* pf;

	CString Url = _T("http://ihamsterball.github.io/Login/LatestVersion.info");
	pf = (CHttpFile*)session.OpenURL(Url);

	char str[1024];
	while (pf->ReadString((LPTSTR)str, 1024))   //读取网页数据 
	{
		cache += str;
	}
	pos = cache.ReverseFind('.');
	CString latestVer = cache.Left(pos);
	CString latestbuild = cache.Mid(pos + 1, cache.GetLength() - pos - 2);

	//如果检查到新版本
	if (latestVer != mainVer || latestVer == mainVer&&build < latestbuild)
	{
		//弹出消息框并获取用户操作
		int ret;
		ret = MessageBox(_T("发现新版本，是否下载？"), _T("提示"), MB_YESNO | MB_ICONASTERISK | MB_TASKMODAL);
		switch (ret)
		{
		case IDYES:
			ShellExecute(NULL, _T("open"), _T("http://ihamsterball.github.io/Login/LatestVersion/Login.exe"), NULL, NULL, SW_SHOWNORMAL);
			break;
		case IDNO:
			break;
		default:
			break;
		}
	}
}


void CLoginDlg::OnAbout()
{
	// TODO:  在此添加控件通知处理程序代码
	CAboutDlg Dlg;
	Dlg.DoModal();
}


//菜单的执行代码
void CLoginDlg::OnMenuAbout()
{
	CAboutDlg Dlg;
	Dlg.DoModal();
}
void CLoginDlg::OnMenuRefresh()
{
	OnRefresh();
}
void CLoginDlg::OnMenuExit()
{
	OnOK();
}
void CLoginDlg::OnOnlineSupport()
{
	ShellExecute(NULL, _T("open"), _T("http://ihamsterball.github.io/Login"), NULL, NULL, SW_SHOWNORMAL);
}
void CLoginDlg::OnMenuCheckUpdate()
{
	OnCheckUpdate();
}
