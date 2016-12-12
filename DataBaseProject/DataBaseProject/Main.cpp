#include "iostream"  
#include "string"  
#include "vector" 
#include "afx.h"
#include "atlstr.h"
#include "HashMap.h"
#include "SimpleBplusTree.h"
#include <algorithm>
#include <queue>
const int MAXN = 1e3;
#import "C:\Program Files\Common Files\System\ado\msado15.dll" no_namespace rename("EOF","adoEOF")  
using namespace std;

BPlusTree POStree("POStree.txt"),POtree("POtree.txt"),PStree("PStree.txt");
SimpleBPlusTree IDtree("IDtree.txt"),SPOtree("SPOtree.txt");
TRecord record;
SearchResult result;

vector<FILEP> vec[MAXN];
int main()
{
	CoInitialize(NULL);          
	_ConnectionPtr pMyConnect(__uuidof(Connection));
	_RecordsetPtr pRst(__uuidof(Recordset));
	try
	{
		pMyConnect->Open("Provider=SQLOLEDB; Server=.;Database=local; uid=sa; pwd=123456;", "", "", adModeUnknown);
	}
	catch (_com_error &e)
	{
		cout << "Initiate failed!" << endl;
		cout << e.Description() << endl;
		cout << e.HelpFile() << endl;
		return 0;
	}
	cout << "Connect succeed!" << endl;
	/*---------------------------------------------�������ݿ�ɹ�-------------------------------------------*/
	string col[4] = { "subject", "property", "object","sub" };
	CString sql;
	int time = clock();  
	sql.Format("select * from Sub");
	pRst = pMyConnect->Execute((_bstr_t)sql, NULL, adCmdText);
	if (!pRst->BOF) pRst->MoveFirst();
	else cout << "Test data is empty!" << endl;
	int N=10000000,p;
	while (!pRst->adoEOF && N--)
	{
		string subject = "";
		if (pRst->GetCollect(col[3].c_str()).vt != VT_NULL)
		{
			subject = (string)(_bstr_t)pRst->GetCollect(col[3].c_str());
		}
		if (subject != "")
		{
			record.key = HashMap(subject, subject);
			record.id = 0;
			IDtree.Insert_BPlus_Tree(record);
		}
		
		pRst->MoveNext();
	}
	cout <<"��ʼ���� "<<clock() - time << endl;
	cout << IDtree.idSum << endl;
	//IDtree.EnumLeafKey();
	/*----------------------------------------------ID��ʼ�����--------------------------------------------*/
	time = clock();
	KEYTYPE Sid, Oid;
	N = 10000000;
	sql.Format("select * from yago");
	pRst = pMyConnect->Execute((_bstr_t)sql, NULL, adCmdText);
	if (!pRst->BOF) pRst->MoveFirst();
	bool flag;
	while (!pRst->adoEOF && N--)
	{
		string tmp[4];
		flag = true;
		for (int i = 0; i < 3; i++)
		{
			if (pRst->GetCollect(col[i].c_str()).vt != VT_NULL)
			{
				tmp[i] = (string)(_bstr_t)pRst->GetCollect(col[i].c_str());
			}
			else flag = false;
		}
		if (flag)
		{
			record.key = HashMap(tmp[2], tmp[2]);
			IDtree.Search_BPlus_Tree(record, result, p);
			if (result.exist) Oid = result.idNum;
			else Oid = 0;

			record.key = HashMap(tmp[0], tmp[0]);
			IDtree.Search_BPlus_Tree(record, result, p);
			Sid = result.idNum;


			//����Ϊʵ�壬����SPO����
			if (Oid!=0)
			{
				//cout << tmp[0] << "  " << tmp[1] << "  " << tmp[2]<<"Oid: "<<Oid<< endl;
				record.key = HashMap(tmp[0], tmp[1]);
				record.id = Oid;
				SPOtree.Insert_BPlus_Tree(record);	
			}

			//����POS����
			record.key = HashMap(tmp[1], tmp[2]);
			record.id = Sid;
			//cout << record.key << " " << record.id << endl;
			// PO���ظ��ұ���Ϊʵ�������� ����PO����
			if (POStree.Insert_BPlus_Tree(record) && Oid != 0)
			{
				record.key = HashMap(tmp[1], tmp[1]);
				record.id = Oid;
				POtree.Insert_BPlus_Tree(record);
			}

			//����PS����
			record.key = HashMap(tmp[1], tmp[1]);
			record.id = Sid;
			PStree.Insert_BPlus_Tree(record);
			
			
		}

		pRst->MoveNext();
	}
	cout <<"��������: "<< clock() - time << endl;
	/*---------------------------------------------------�����������-------------------------------------------*/
	int cnt = 0;
	string sub, pro, obj;
	IDtree.~SimpleBPlusTree();
	SPOtree.~SimpleBPlusTree();
	POStree.~BPlusTree();
	POtree.~BPlusTree();
	PStree.~BPlusTree();
	while (true)
	{
		printf("������Ҫ��ѯ����ߣ�\n");
		
		while (true)
		{
			cout << "������ȷ����:\n";
			cin >> sub >> pro;
			if (sub=="0"||pro == "0") break;
			record.key = HashMap(sub, pro);
			SPOtree.Search_BPlus_Tree(record,result,p);
			if (result.exist)
			{
				cout << "Find it:  " <<result.idNum<< endl;
			}
		}
		while (true)
		{
			cout << "�����벻ȷ����:\n";
			cin >> pro;
			if (pro == "0") break;
			record.key = HashMap(pro, pro);
			POtree.Search_BPlus_Tree(record, result, p);
			if (result.exist)
			{
				vec[cnt] = POtree.ReadF(result);
				cout << vec[cnt].size() << endl;
				//for (int i = 0; i < vec[cnt].size(); i++) cout << vec[cnt][i] << " ";
				cout << endl;
			}
		}

		printf("������Ҫ��ѯ�ĳ��ߣ�\n");
		while (true)
		{
			cout << "������ȷ����:\n";
			cin >> pro >> obj;
			if (obj == "0" || pro == "0") break;
			record.key = HashMap(pro, obj);
			POStree.Search_BPlus_Tree(record, result, p);
			if (result.exist)
			{
				vec[cnt] = POStree.ReadF(result);
				cout << vec[cnt].size() << endl;
				//for (int i = 0; i < vec[cnt].size(); i++) cout << vec[cnt][i] << " ";
				cout << endl;
			}
		}
		while (true)
		{
			cout << "�����벻ȷ����:\n";
			cin >> pro;
			if (pro == "0") break;
			record.key = HashMap(pro, pro);
			PStree.Search_BPlus_Tree(record, result, p);
			if (result.exist)
			{
				vec[cnt] = PStree.ReadF(result);
				cout << vec[cnt].size() << endl;
				//for (int i = 0; i < vec[cnt].size(); i++) cout << vec[cnt][i] << " ";
				cout << endl;
			}
		}

	}
	return 0;
}
//struct Node
//{
//	double cost;
//	int id;
//	Node(double _id, int _cost) :cost(_cost), id(_id){}
//	bool operator < (Node A) const
//	{
//		return cost < A.cost;
//	}
//};
//vector <Node> tmp;
//priority_queue <Node> topk;
//vector <FILEP> vec[1000];
//vector<FILEP>::iterator iter;
//
//ע�����ö��ַ��� �Լ� MFC�����BE SAFE, MFC���ַ������ 
//
//
//void FindTopK(int n)
//{
//	for (int i = 0; i < n; i++) sort(vec[i].begin(), vec[i].end());
//	for (int i = 0; i < n; i++)
//	{
//		for (int j = 0; j < vec[i].size(); j++)
//		{
//			cout << vec[i][j] << " ";
//		}
//		cout << endl;
//	}
//	getchar();
//	/*
//	int k = 3, cnt = 0;
//	double maxx = 1;
//	sort(tmp.begin(), tmp.end());
//	for (int i = 0; i < n; i++)
//	{
//		int id = tmp[i].id;
//		for (int j = 0; j < vec[id].size(); j++)
//		{
//			int key = vec[id][j];
//			double score = 0;
//			if (cnt < k)
//			{
//				for (int k = i + 1; k <= n; k++)
//				{
//					iter=lower_bound(vec[tmp[k].id].begin(), vec[tmp[k].id].end(), key);
//					if (iter != vec[tmp[k].id].end() && vec[tmp[k].id][iter - vec[tmp[k].id].begin()] == key)
//					{
//						score += tmp[k].cost;
//					}
//				}
//				topk.push(Node(key, score));
//			}
//			else
//			{
//				if (maxx < topk.top().cost) break;
//				for (int k = i + 1; k <= n; k++)
//				{
//					iter = lower_bound(vec[tmp[k].id].begin(), vec[tmp[k].id].end(), key);
//					if (iter != vec[tmp[k].id].end() && vec[tmp[k].id][iter - vec[tmp[k].id].begin()] == key)
//					{
//						score += tmp[k].cost;
//					}
//				}
//				if (score>topk.top().cost) topk.push(Node(key, score));
//			}
//		}
//	}*/
//}
//diedOnDate "1970-##-##"^^xsd:date wasBornOnDate "1979-##-##"^^xsd:date
//void  InputKey()
//{
//	string property, object;
//	double score;
//	int cnt = 0, p,flag;
//	printf("Please input your search:\n");
//	while (cin >> property >> object)
//	{
//		cout << property << object;
//		record.key = HashMap(property, object);
//		cout << record.key << endl;
//		tree.Search_BPlus_Tree(record, result, p);
//		if (result.exist)
//		{
//			vec[cnt] = tree.ReadF(result);
//			for (int i = 0; i < vec[cnt].size(); i++) cout << vec[cnt][i] << " ";
//			cout << endl;
//			cnt++;
//		}
//		if (object == "0") break;
//	}
//	FindTopK(cnt);
//}
//
//int main()
//{
//	CoInitialize(NULL);          
//	_ConnectionPtr pMyConnect(__uuidof(Connection));
//	_RecordsetPtr pRst(__uuidof(Recordset));
//	_RecordsetPtr pOst(__uuidof(Recordset)); 
//	try
//	{
//		pMyConnect->Open("Provider=SQLOLEDB; Server=.;Database=local; uid=sa; pwd=123456;", "", "", adModeUnknown);
//	}
//	catch (_com_error &e)
//	{
//		cout << "Initiate failed!" << endl;
//		cout << e.Description() << endl;
//		cout << e.HelpFile() << endl;
//		return 0;
//	}
//	cout << "Connect succeed!" << endl;
//	/*---------------------------------------------�������ݿ�ɹ�-------------------------------------------*/
//	vector<_bstr_t> col;
//	CString sql;
//	sql.Format("select * from HEHE");
//	pRst = pMyConnect->Execute((_bstr_t)sql, NULL, adCmdText);      
//	if (!pRst->BOF) pRst->MoveFirst();
//	else cout << "Test data is empty!" << endl;
//	int N=10;
//
//	for (int i = 0; i< pRst->Fields->GetCount(); i++)
//	{
//		col.push_back(pRst->Fields->GetItem(_variant_t((long)i))->Name);
//	}
//	int SubNum=0,N=300;
//	/*�Ա���б�������,��ʾ����ÿһ�е�����*/
//
//	while (!pRst->adoEOF && N--)
//	{
//		string tmp[4];
//		bool flag = true;
//		vector<_bstr_t>::iterator iter = col.begin();
//		int i = 0;
//		for (iter; iter != col.end(); iter++,i++)
//		{
//			if (pRst->GetCollect(*iter).vt != VT_NULL)
//			{
//				tmp[i] = (string)(_bstr_t)pRst->GetCollect(*iter);
//			}
//			else flag = false;
//		}
//		if (flag)
//		{
//			string subject = tmp[0];
//			sql.Format("select id from Sub where sub = \'%s\'", subject.c_str());
//			pOst = pMyConnect->Execute((_bstr_t)sql, NULL, adCmdText);
//			if (!pOst->BOF) record.id = pOst->GetCollect("id");
//			else
//			{
//				sql.Format("insert into Sub values('%s')",subject.c_str());
//				pMyConnect->Execute((_bstr_t)sql, NULL, adCmdText);
//				SubNum++;
//				record.id = SubNum;
//			}
//			
//			cout << record.id << endl;
//			record.key = HashMap(tmp[1], tmp[2]);
//			cout << tmp[1] << tmp[2] << record.key << endl;
//			tree.Insert_BPlus_Tree(record);
//		}
//		pRst->MoveNext();
//	}
//	����4���ر�����Դ
//	/*�ر����ݿⲢ�ͷ�ָ��*/
//	tree.~BPlusTree();
//	try
//	{
//		pRst->Close();     //�رռ�¼��               
//		pMyConnect->Close();//�ر����ݿ�               
//		pRst.Release();//�ͷż�¼������ָ��               
//		pMyConnect.Release();//�ͷ����Ӷ���ָ��
//	}
//	catch (_com_error &e)
//	{
//		cout << e.Description() << endl;
//		cout << e.HelpFile() << endl;
//		return 0;
//	}
//	CoUninitialize(); //�ͷ�COM����
//	
//	tree.EnumLeafKey();
//	InputKey();
//	return 0;
//}