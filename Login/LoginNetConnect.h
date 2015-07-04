#include "stdafx.h"
#include <afxinet.h>
#include <exception>
class LoginNetConnect
{
private:
	CString cache = _T("");
public:
	//�ӿ�
	void OnBjfuLogin(
		CStringA &username, // username
		CStringA &password, //password
		CStringA &ip, //ip
		CStringA &action //action
		);//TODO: ������Ǹ���������ʱ������Ҹ�������д -- ����д

	inline void Empty(){ Account = ""; Password = ""; }

	//����
	CStringA Account;//�˺�
	CStringA Password;//����
	int userid;//ò����һ��Ψһ��userid���ں�������

	CString ConnectStatus;//����״̬
	double AccountBalance;//�˻����
	double DataUsage;//��ʹ������
	double FreeDataBalance;//ʣ���������
	double BasicDataTotal;//�ܵĻ�������
	double BasicDataBalance;//ʣ���������
	double BasicDataExceed;//������������
	double ExceedDataFee;//����������

	//�ڲ���������������
	void Request(CHttpConnection* pServer, CHttpFile* pf,
		CString Method, CString RequestPath, CString Referer, CString Host,
		CStringA data);

	//�ڲ����ݴ�����
	void DataGather(CString &data);
	//�ڲ����ݴ����� +1����
	void DataGather(double &data, CString keyword, CString StartStr);
	//�ڲ����ݴ����� +2����
	void DataGather(double &data, CString keyword, CString StartStr, CString EndStr);

	//����sort������<���������
	bool operator < (const LoginNetConnect &a)const
	{
		return FreeDataBalance > a.FreeDataBalance ||
			(FreeDataBalance == a.FreeDataBalance && BasicDataBalance > a.BasicDataBalance);
	}
};