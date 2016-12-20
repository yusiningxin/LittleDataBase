#include "iostream"  
#include "string"  
#include "vector" 
#include "afx.h"
#include "atlstr.h"
#include "HashMap.h"
#include "SimpleBplusTree.h"
#include <algorithm>
#include <queue>
#include <set>
const int MAXN = 1e3;
#import "C:\Program Files\Common Files\System\ado\msado15.dll" no_namespace rename("EOF","adoEOF")  
using namespace std;

BPlusTree POStree("POStree.txt"),POtree("POtree.txt"),PStree("PStree.txt");
SimpleBPlusTree IDtree("IDtree.txt"),SPOtree("SPOtree.txt");
TRecord record;
SearchResult result;

set <FILEP> st;
struct TOPKEY
{
	vector<FILEP> vec;
	double score;
	int type;
}key[MAXN];

struct NodeUP
{
	double down;
	double up;
	FILEP id;
	NodeUP(FILEP _id, double _down,double _up) :id(_id), down(_down), up(_up){}
	bool operator < (NodeUP A) const //��С��������
	{
		return up > A.up;
	}
};
struct NodeDOWN
{
	double down;
	double up;
	FILEP id;
	NodeDOWN(FILEP _id, double _down, double _up) :id(_id), down(_down), up(_up){}
	bool operator < (NodeDOWN A) const //���޴�С�������У�������ͬ���޴�С������
	{
		if (down!=A.down) return down > A.down;
		return up > A.up;
	}
};
priority_queue <NodeDOWN> topk;
priority_queue <NodeUP> extra;
vector<FILEP>::iterator iter;
struct INDEX
{
	double score;
	int id;
	INDEX(int _id, double _score) :score(_score), id(_id){}
	bool operator < (INDEX A) const
	{
		return score > A.score;
	}
};
vector <INDEX> tmp;

void FindTopK(int n)
{
	tmp.clear();
	int K = 10,cnt=0;
	double maxx=0,up,down;
	for (int i = 1; i <= n; i++) maxx+=key[i].score,sort(key[i].vec.begin(), key[i].vec.end()),tmp.push_back(INDEX(i,key[i].score));
	sort(tmp.begin(), tmp.end());

	for (int i = 1; i <= n; i++)
	{
		int id = tmp[i].id;
		for (int j = 0; j < key[id].vec.size(); j++)
		{
			int  a = key[id].vec[j];
			up = down = 0;
			if (cnt < K)
			{
				for (int k = i + 1; k <= n; k++)
				{
					iter = lower_bound(key[tmp[k].id].vec.begin(), key[tmp[k].id].vec.end(), a);
					if (iter != key[tmp[k].id].vec.end() && key[tmp[k].id].vec[iter - key[tmp[k].id].vec.begin()] == a)
					{
						if (key[tmp[k].id].type == 1 || key[tmp[k].id].type == 3) down += tmp[k].score, up += tmp[k].score;
						else up += tmp[k].score;
					}
				}
				topk.push(NodeDOWN(a,down,up));
				cnt++;
			}
			else
			{
				if (maxx < topk.top().down) break;
				for (int k = i + 1; k <= n; k++)
				{
					iter = lower_bound(key[tmp[k].id].vec.begin(), key[tmp[k].id].vec.end(), a);
					if (iter != key[tmp[k].id].vec.end() && key[tmp[k].id].vec[iter - key[tmp[k].id].vec.begin()] == a)
					{
						if (key[tmp[k].id].type == 1 || key[tmp[k].id].type == 3) down += tmp[k].score, up += tmp[k].score;
						else up += tmp[k].score;
					}
				}
				if (down > topk.top().down)
				{
					topk.push(NodeDOWN(a, down, up));
					cnt++;
					while (cnt - K) topk.pop(),cnt--;
				}
				else if (down == topk.top().down)
				{
					topk.push(NodeDOWN(a, down, up));
					cnt++;
				}
				else if (up >= topk.top().down)
				{
					extra.push(NodeUP(a, down, up));
				}
				while (extra.top().up < topk.top().down) extra.pop();
			}
		}
	}
}
void Solve()
{
	string sub, pro, obj;
	int p,cnt = 0,sum;
	printf("�������ܹ�Ҫ��ѯ��ʵ�������\n");
	cin >> sum;
	while (true)
	{
		cnt++;
		printf("��ѯ��%d��ʵ�壺\n", cnt);
		int num=0, flag,Only = false;
		st.clear();
		while (true)
		{
			num++;
			printf("��ѡ����֪�ߵ����ͣ�\n1--������ȷ����  2--�����Ҳ�ȷ����  3--�����ȷ����  4--����Ҳ�ȷ����\n");
			cin >> key[num].type;
			if (key[num].type == 1)
			{
				printf("�Ƿ�ΪΨһ���ԣ�1--Ψһ����  0--��Ψһ����\n");
				cin >> flag;
				if (flag) Only = true;
				printf("������ν�� �����Լ��ߵ�Ȩ�أ� ");
				cin >> pro >> obj;
				cin >> key[num].score;
				key[num].vec.clear();
				record.key = HashMap(pro, obj);
				POStree.Search_BPlus_Tree(record, result, p);
				if (result.exist)
				{
					if (Only)
					{
						set<FILEP> t;
						key[num].vec = POStree.ReadF_Only(result, st);
						record.key = HashMap(pro, pro);
						PStree.Search_BPlus_Tree(record, result, p);
						if (result.exist)
						{
							for (int i = 0; i < key[num].vec.size(); i++) t.insert(key[num].vec[i]);
							PStree.Select(result, st,t);
						}
					}
					else key[num].vec = POStree.ReadF(result);

				}
			}
			else if (key[num].type == 2)
			{
				printf("������ν�ʼ��ߵ�Ȩ�أ� ");
				cin >> pro;
				cin >> key[num].score;
				key[num].vec.clear();
				record.key = HashMap(pro, obj);
				PStree.Search_BPlus_Tree(record, result, p);
				if (result.exist)
				{
					if (Only) key[num].vec = PStree.ReadF_Only(result, st);
					else key[num].vec = PStree.ReadF(result);
				}
			}
			else if (key[num].type == 3)
			{
				printf("���������� ν�ʼ��ߵ�Ȩ�أ� ");
				cin >> sub >> pro;
				cin >> key[num].score;
				key[num].vec.clear();
				record.key = HashMap(sub, pro);
				SPOtree.Search_BPlus_Tree(record, result, p);
				if (result.exist)
				{
					if (!(Only && st.find(result.idNum) != st.end()))  key[num].vec.push_back(result.idNum);
				}
			}
			else if (key[num].type == 4)
			{
				printf("������ν�ʼ��ߵ�Ȩ�أ� ");
				cin >> pro;
				cin >> key[num].score;
				key[num].vec.clear();
				record.key = HashMap(pro, pro);
				POtree.Search_BPlus_Tree(record, result, p);
				if (result.exist)
				{
					if (Only) key[num].vec = POtree.ReadF_Only(result, st);
					else key[num].vec = POtree.ReadF(result);
				}
			}
			else break;
		}
		num--;
		FindTopK(num);
		if (cnt == sum) break;
	}
	
}

//ע�����ö��ַ��� �Լ� MFC�����BE SAFE, MFC���ַ������ 
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
	IDtree.~SimpleBPlusTree();
	SPOtree.~SimpleBPlusTree();
	POStree.~BPlusTree();
	POtree.~BPlusTree();
	PStree.~BPlusTree();

	/*---------------------------------------------------�����������-------------------------------------------*/
	Solve();

	return 0;
}