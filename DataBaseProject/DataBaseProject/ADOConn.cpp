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
		// 初始化OLE/COM库环境
		::CoInitialize(NULL);
		try
		{
			// 创建数据库连接对象
			m_pConnection.CreateInstance(__uuidof(Connection));
			_bstr_t strConnect = "driver={SQL Server};Server=.;DATABASE=local;uid=sa;pwd=123456;";
			//设置等待连接打开的时间为20s
			m_pConnection->ConnectionTimeout = 0;
			m_pConnection->CommandTimeout = 9999999;

			m_pConnection->Open(strConnect, "", "", adConnectUnspecified);
		}
		//捕捉异常
		catch (_com_error e)
		{
			//显示错误信息
			//	AfxMessageBox(e.Description());
		}
		cout << "数据库连接成功" << endl;
	}
}


void ADOConn::ExitConnect()
{
	// 关闭记录集和连接
	m_pConnection->Close();
	// 释放环境资源
	::CoUninitialize();
}

