// ADOConn.h: interface for the ADOConn class.
//
//////////////////////////////////////////////////////////////////////

#import "C:\Program Files\Common Files\System\ado\msado15.dll" no_namespace rename("EOF","adoEOF")  
#if !defined(AFX_ADOCONN_H__24BC13CF_96ED_4FF8_9A38_6A362E6B8DEF__INCLUDED_)
#define AFX_ADOCONN_H__24BC13CF_96ED_4FF8_9A38_6A362E6B8DEF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class ADOConn
{
public:
	_ConnectionPtr m_pConnection; // 数据库连接对象的指针
public:
	ADOConn();
	virtual ~ADOConn();
	void OnInitADOConn(); // 初始化数据库连接
	void ExitConnect(); // 断开与数据库的连接

};

#endif // !defined(AFX_ADOCONN_H__24BC13CF_96ED_4FF8_9A38_6A362E6B8DEF__INCLUDED_)