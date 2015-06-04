
// LoginDlg.cpp : ʵ���ļ�
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

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	CString ver = GetAppVersion(L"Login.exe");
	if (ver.IsEmpty()) return FALSE;

	int pos = ver.ReverseFind('.');
	CString mainVer = ver.Left(pos);
	CString build = ver.Right(ver.GetLength() - pos - 1);
	GetDlgItem(IDC_STATIC_VER)->SetWindowText(mainVer);
	GetDlgItem(IDC_STATIC_BUILD)->SetWindowText(build);
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣:  OCX ����ҳӦ���� FALSE
}
BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CLoginDlg �Ի���

//ȫ������
//LoginNetConnect Connection[6];//������������
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
	List.InsertColumn(3, _T("ʣ���������"), LVCFMT_CENTER, 100);
	List.InsertColumn(4, _T("ʣ���������"), LVCFMT_CENTER, 100);
	List.InsertColumn(5, _T("������������"), LVCFMT_CENTER, 100);
	List.InsertColumn(6, _T("����������"), LVCFMT_CENTER, 80);
	List.InsertColumn(7, _T("��ʹ������"), LVCFMT_CENTER, 80);
	List.InsertColumn(8, _T("�ײ�����"), LVCFMT_CENTER, 80);
	//List.InsertColumn(4, _T("����"), LVCFMT_CENTER, 150);

	//��ȡ�˺���Ϣ
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

	//��ʼ���б�����
	OnRefresh();
	std::sort(vec.begin(), vec.end());
	for (cur = vec.begin(); cur != vec.end();cur++)//δ������ʹ��vector�䳤�������
		if ((*cur).ConnectStatus == _T("�û����ڱ������ϵ�¼������Ҫ���µ�¼")
			|| (*cur).ConnectStatus == _T("�����ɹ�"))
		{
			(*cur).OnBjfuLogin((*cur).Account, (*cur).Password, (CStringA)"118.228.171.91", (CStringA)"disconnect");
			break;
		}
	//��Ҫ�����������������������
	for (cur = vec.begin(); cur != vec.end(); cur++)
	{
		if ((*cur).BasicDataBalance > 0)
			(*cur).OnBjfuLogin((*cur).Account, (*cur).Password, (CStringA)"118.228.171.91", (CStringA)"connect");
		if (!((*cur).ConnectStatus == _T("�û����ڱ������ϵ�¼������Ҫ���µ�¼")
			|| (*cur).ConnectStatus == _T("�����ɹ�")))
			continue;
		else
			break;
	}
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
	CStringA ip;//�ڵ�ǰ�汾�У�����Ҫ�ֶ�ָ��IP��ַ��������Զ���ȡ��ǰ���������IP��ַ
	CStringA action;
	action = _T("admin");
	List.DeleteAllItems();
	for (cur = vec.begin(); cur != vec.end(); cur++)
	{
		int tmp = List.InsertItem(0, (CString)(*cur).Account);
		(*cur).OnBjfuLogin((*cur).Account, (*cur).Password, ip, action);

		CString TempStr;

		if ((*cur).ConnectStatus == _T("�û����ڱ������ϵ�¼������Ҫ���µ�¼")
			|| (*cur).ConnectStatus == _T("�����ɹ�"))
			TempStr = _T("��");
		else if ((*cur).ConnectStatus == _T("�Բ��������û�״̬����ȷ�������ڵ�״̬�ǣ�[Ƿ��]���������Ա��ϵ��"))
			TempStr = _T("��");
		else if ((*cur).ConnectStatus == _T("�������"))
			TempStr = _T("Error 1");
		else
			TempStr = _T("");
		List.SetItemText(tmp, 1, TempStr);

		if (TempStr != _T("Error 1"))
		{
			TempStr.Format(_T("%.3lf"), (*cur).AccountBalance);
			TempStr.AppendFormat(_T("Ԫ"));
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
			TempStr.AppendFormat(_T("Ԫ"));
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
	//��ȡ��ǰ�ļ��İ汾��Ϣ
	CAboutDlg Dlg;
	CString ver = Dlg.GetAppVersion(_T("Login.exe"));
	if (ver.IsEmpty()) return;

	int pos = ver.ReverseFind('.');
	CString mainVer = ver.Left(pos);
	CString build = ver.Right(ver.GetLength() - pos - 1);

	//��ȡ���¸�����Ϣ
	CString cache = _T(""); 
	CInternetSession  session;
	CHttpFile* pf;

	CString Url = _T("http://ihamsterball.github.io/Login/LatestVersion.info");
	pf = (CHttpFile*)session.OpenURL(Url);

	char str[1024];
	while (pf->ReadString((LPTSTR)str, 1024))   //��ȡ��ҳ���� 
	{
		cache += str;
	}
	pos = cache.ReverseFind('.');
	CString latestVer = cache.Left(pos);
	CString latestbuild = cache.Mid(pos + 1, cache.GetLength() - pos - 2);

	//�����鵽�°汾
	if (latestVer != mainVer || latestVer == mainVer&&build < latestbuild)
	{
		//������Ϣ�򲢻�ȡ�û�����
		int ret;
		ret = MessageBox(_T("�����°汾���Ƿ����أ�"), _T("��ʾ"), MB_YESNO | MB_ICONASTERISK | MB_TASKMODAL);
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
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CAboutDlg Dlg;
	Dlg.DoModal();
}


//�˵���ִ�д���
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
