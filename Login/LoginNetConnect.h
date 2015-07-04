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
		CStringA &username, // �û���
		CStringA &password, // ����
		CStringA &ip, // IP
		CStringA &action // ����
		);//TODO: ������Ǹ���������ʱ������Ҹ�������д -- ����д
	void OnGatherTrafficLog(CString from_time, CString to_time, CString IPAddress);//��ȡ������־

	inline void Empty(){ Account = ""; Password = ""; }

	//����
	CStringA Account;//�˺�
	CStringA Password;//����
	CStringA IP;//IP����֮ǰΪ��û�ӣ��s���ߣ����s�k�k��
	int userid;//ò����һ��Ψһ��userid���ں�������

	CString ConnectStatus;//����״̬
	double AccountBalance;//�˻����
	double DataUsage;//��ʹ������
	double FreeDataBalance;//ʣ���������
	double BasicDataTotal;//�ܵĻ�������
	double BasicDataBalance;//ʣ���������
	double BasicDataExceed;//������������
	double ExceedDataFee;//����������

	int pages;//������־ҳ��
	int records;//�ܼ���
	std::vector<std::pair<double, double>> vec;//���ڴ����Ҫ����ͼ�������
	std::vector<std::pair<double, double>>::iterator cur;
	void SaveData();

	//�ڲ���������������
	void Request(CHttpConnection* pServer, CHttpFile* pf, CStringA data,
		CString Method, CString RequestPath, CString Referer, CString Host);

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