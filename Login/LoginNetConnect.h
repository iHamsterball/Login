#include "stdafx.h"
#include <afxinet.h>
#include <exception>
class LoginNetConnect
{
private:
	CString cache = _T("");
public:
	//接口
	void OnBjfuLogin(
		CStringA &username, // username
		CStringA &password, //password
		CStringA &ip, //ip
		CStringA &action //action
		);
	inline void Empty(){ Account = ""; Password = ""; }

	//数据
	CStringA Account;//账号
	CStringA Password;//密码

	CString ConnectStatus;//连接状态
	double AccountBalance;//账户余额
	double DataUsage;//已使用流量
	double FreeDataBalance;//剩余免费流量
	double BasicDataTotal;//总的基础流量
	double BasicDataBalance;//剩余基础流量
	double BasicDataExceed;//超出基础流量
	double ExceedDataFee;//超流量费用

	//内部数据处理函数
	void DataGather(CString &data);
	//内部数据处理函数 +1重载
	void DataGather(double &data, CString keyword, CString StartStr);
	//内部数据处理函数 +2重载
	void DataGather(double &data, CString keyword, CString StartStr, CString EndStr);

	//用于sort函数的<运算符重载
	bool operator < (const LoginNetConnect &a)const
	{
		return FreeDataBalance > a.FreeDataBalance ||
			(FreeDataBalance == a.FreeDataBalance && BasicDataBalance > a.BasicDataBalance);
	}
};