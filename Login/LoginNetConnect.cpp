#include "stdafx.h"
#include "LoginNetConnect.h"

//网页数据请求函数
void LoginNetConnect::Request(CHttpConnection* pServer, CHttpFile* pf, CStringA data,
	CString Method, CString RequestPath, CString Referer, CString Host)
{
	//拼接头部
	pf = pServer->OpenRequest((Method == _T("POST")) ? CHttpConnection::HTTP_VERB_POST : CHttpConnection::HTTP_VERB_GET, RequestPath);
	pf->AddRequestHeaders(_T("Accept: text/html, application/xhtml+xml, */*"));
	pf->AddRequestHeaders(_T("Referer: ") + Referer);
	pf->AddRequestHeaders(_T("Accept-Language: zh-Hans-CN,zh-Hans;q=0.8,en-GB;q=0.5,en;q=0.3"));
	pf->AddRequestHeaders(_T("User-Agent: Mozilla/5.0 (Windows NT 6.3; WOW64; Trident/7.0; rv:11.0) like Gecko"));
	pf->AddRequestHeaders(_T("Content-Type: application/x-www-form-urlencoded"));
	pf->AddRequestHeaders(_T("Host: ") + Host);
	pf->AddRequestHeaders(_T("DNT: 1"));
	pf->AddRequestHeaders(_T("Connection: Keep-Alive"));
	pf->AddRequestHeaders(_T("Cache-Control: no-cache"));

	pf->SendRequestEx(data.GetLength());
	pf->WriteString((LPCTSTR)data.GetBuffer());
	pf->EndRequest();

	DWORD statusCode = 0;
	if (!pf->QueryInfoStatusCode(statusCode) || statusCode != HTTP_STATUS_OK)
	{
		CString e;
		e.Format(_T("网页响应代码%d\n"), statusCode);
		MessageBox(0, e, _T("错误"), 0);
	}

	// 以下为读取数据
	cache = _T(""); 
	char str[1024];
	while (pf->ReadString((LPTSTR)str, 1024))   //读取网页数据 
	{
		cache += str;
	}

	//关闭HttpFile
	if (pf != NULL)
	{
		pf->Close();
		delete pf;
		pf = NULL;
	}
}

// 模拟IE11登陆北林登陆网页
void LoginNetConnect::OnBjfuLogin(
	CStringA &username, // username
	CStringA &password, //password
	CStringA &ip, //ip
	CStringA &action //action
	)
{
	Account = username;
	Password = password;
	CInternetSession  session;
	CHttpConnection* pServer;
	CHttpFile* pf;

	CString ServerName = _T("202.204.122.1");
	INTERNET_PORT nPort = 80;  //port
	pServer = session.GetHttpConnection(ServerName, nPort); 
	pf = pServer->OpenRequest(CHttpConnection::HTTP_VERB_GET, _T("/"));

	CStringA temp = "username=" + username + "&password=" + password + "&ip=" + ip + "&action=" + action;

	Request(pServer, pf, temp, _T("GET"), _T("/"), _T(""), _T(""));
	//Package -1
	//读取IP地址
	{
		int start, end;
		start = cache.Find(_T("name=\"ip\""));
		cache = cache.Right(cache.GetLength() - start - 10);
		start = cache.Find(_T("\""));
		end = cache.Find(_T("\">"));
		ip = cache.Mid(start + 1, end - start - 1);
	}
	IP = ip;
	CString CookieData;
	//获取Cookie
	session.GetCookie(_T("http://202.204.122.1/checkLogin.jsp"), _T("JSESSIONID"), CookieData);
	int err = GetLastError();
	//设置Cookie
	session.SetCookie(_T("http://202.204.122.1"), _T(""), CookieData);
	

	//获取IP地址(Package 1)后才能构造提交数据
	//POST /checkLogin.jsp -package 2 
	//Login to system
	CStringA data = "username=" + username + "&password=" + password + "&ip=" + ip + "&action=" + action;

	Request(pServer, pf, data, 
		_T("POST"), 
		_T("/checkLogin.jsp"), 
		_T("http://202.204.122.1/index.jsp"), 
		_T("202.204.122.1"));
	//检查用户名密码
	if (cache.Find(_T("用户名或密码不正确")) != -1)
	{
		ConnectStatus = _T("密码错误");
		//关闭HttpFile
		if (pf != NULL)
		{
			pf->Close();
			delete pf;
			pf = NULL;
		}
		//断开与服务器连接
		if (pServer != NULL)
		{
			pServer->Close();
			delete pServer;
			pServer = NULL;
		}
		session.Close();
		return;
	}

	//通过cache中的src确定下一个数据包请求的地址
	//不同用户，userid不同
	//GET /user/index.jsp?ip="IP Address"&action=connect -package 3
	Request(pServer, pf, data,
		_T("GET"),
		_T("/user/index.jsp?ip=") + (CString)ip + _T("&action=") + ((action == "disconnect") ? _T("disconnect") : _T("connect")),
		_T("http://202.204.122.1/index.jsp"),
		_T("202.204.122.1"));
	CString RequestURL;
	{
		int start, end;
		start = cache.Find(_T("</frameset>"));
		end = cache.Find(_T("\""), start + 31);
		RequestURL = cache.Mid(start + 31, end - start - 31);
		start = cache.Find(_T("userid="));
		end = cache.Find(_T("&ip"), start);
		userid = _ttoi(cache.Mid(start + 7, end - start - 7));

		//MessageBox(cache, _T("网页信息"), 0);
	}

	//GET /user/+RequestURL -package 4
	//收集需要的信息
	Request(pServer, pf, data,
		_T("GET"),
		_T("/user/") + RequestURL,
		_T("http://202.204.122.1/user/index.jsp?ip=") + (CString)ip + _T("&action") + ((action == "disconnect") ? _T("disconnect") : _T("connect")),
		_T("202.204.122.1"));
	if (action != "disconnect")
	{
		//处理cache以获取需要的信息
		DataGather(ConnectStatus);//连接状态
		DataGather(AccountBalance, _T("账户余额"), _T("FF0000"), _T("</font>"));//账户余额
		DataGather(DataUsage, _T("本月产生流量"), _T("nowrap\">"), _T("&nbsp;GB"));//已使用流量
		DataGather(FreeDataBalance, _T("剩余免费流量"), _T("nowrap\">"), _T("&nbsp;GB"));//剩余免费流量
		DataGather(BasicDataTotal, _T("总的基础流量"), _T("nowrap\">"), _T("&nbsp;GB"));//总的基础流量
		DataGather(BasicDataBalance, _T("剩余基础流量"), _T("nowrap\">"), _T("&nbsp;GB"));//剩余基础流量
		DataGather(BasicDataExceed, _T("超出基础流量"), _T("FF0000"), _T("&nbsp;GB"));//超出基础流量
		DataGather(ExceedDataFee, _T("超流量费用"), _T("FF0000"), _T("</font>"));//超流量费用
	}

	//关闭HttpFile
	if (pf != NULL)
	{
		pf->Close();
		delete pf;
		pf = NULL;
	}
	//断开与服务器的连接
	if (pServer != NULL)
	{
		pServer->Close();
		delete pServer;
		pServer = NULL;
	}
	session.Close();
}
void LoginNetConnect::DataGather(CString &data)
{
	int start, end;
	start = cache.Find(_T("连接提示"));
	cache = cache.Right(cache.GetLength() - start);
	start = 23;
	end = cache.Find(_T("</font>"));
	data = cache.Mid(start, end - start);
}
void LoginNetConnect::DataGather(double &data, CString keyword, CString StartStr, CString EndStr)
{
	int start, end;
	start = cache.Find(keyword);
	cache = cache.Right(cache.GetLength() - start);
	start = cache.Find(StartStr);
	end = cache.Find(EndStr);
	data = _ttof(cache.Mid(start + 8, end - start - 8));
}