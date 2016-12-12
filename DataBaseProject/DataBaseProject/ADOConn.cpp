#include "ADOConn.h"
#include <iostream>
using namespace std;
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

ADOConn::ADOConn()
{

}

ADOConn::~ADOConn()
{

}
void ADOConn::OnInitADOConn()
{
	if (m_pConnection == NULL)
	{
		// ��ʼ��OLE/COM�⻷��
		::CoInitialize(NULL);
		try
		{
			// �������ݿ����Ӷ���
			m_pConnection.CreateInstance(__uuidof(Connection));
			_bstr_t strConnect = "driver={SQL Server};Server=.;DATABASE=local;uid=sa;pwd=123456;";
			//���õȴ����Ӵ򿪵�ʱ��Ϊ20s
			m_pConnection->ConnectionTimeout = 0;
			m_pConnection->CommandTimeout = 9999999;

			m_pConnection->Open(strConnect, "", "", adConnectUnspecified);
		}
		//��׽�쳣
		catch (_com_error e)
		{
			//��ʾ������Ϣ
			//	AfxMessageBox(e.Description());
		}
		cout << "���ݿ����ӳɹ�" << endl;
	}
}


void ADOConn::ExitConnect()
{
	// �رռ�¼��������
	m_pConnection->Close();
	// �ͷŻ�����Դ
	::CoUninitialize();
}

