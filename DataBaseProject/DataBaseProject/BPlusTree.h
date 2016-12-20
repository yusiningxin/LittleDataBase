#include<iostream>  
#include<time.h> 
#include<cstdlib> 
#include<string>
#include<vector>
#include<set>
using namespace std;


#define MAX_KEY 255   //B+���Ľ�,����Ϊ����3����  
typedef  __int64  KEYTYPE;
typedef  __int64  FILEP;
//B+���ڵ�����ݽṹ 

struct  BPlusNode
{
	KEYTYPE key[MAX_KEY];      //�ؼ�����  
	FILEP Pointer[MAX_KEY + 1];    //ָ����  
	int   nkey;              //�ؼ�����  
	bool  isleaf;                //�Ƿ�ΪҶ�ڵ� Ҷ�ڵ�:true ����Ϊfalse  
};

//����ؼ��ֵ����ݽṹ  
struct  TRecord
{
	KEYTYPE   key; //�ü�¼�Ĺؼ���
	FILEP Raddress;         //�ùؼ��ֶ�Ӧ��¼�ĵ�ַ
	FILEP id;
};

//�����ѯ��������ݽṹ  
struct    SearchResult
{
	bool  exist;
	FILEP Baddress;   //��������ü�¼��B+���ڵ��ַ  
	FILEP Raddress;   //�ùؼ��ֵ���ָ��ļ�¼��ַ
	FILEP idNum;
};

class BPlusTree
{

	FILEP ROOT;       //�������ļ��ڵ�ƫ�Ƶ�ַ  
	FILEP FROOT;
	FILE  *Bfile;     //B+���ļ���ָ��  
	FILE  *Rfile;     //��¼�ļ���ָ��  

public:

	FILEP GetBPlusNode() const;
	FILEP GetFNode() const;
	void  ReadBPlusNode(const FILEP, BPlusNode&) const;
	void  WriteBPlusNode(const FILEP, const BPlusNode&);

	void  Build_BPlus_Tree();

	bool  Insert_BPlus_Tree(TRecord&);
	bool  insert_bplus_tree(FILEP, TRecord&);

	void  Split_BPlus_Node(BPlusNode&, BPlusNode&, const int);

	void  Search_BPlus_Tree(TRecord&, SearchResult&, int&) const;

	void  Delete_BPlus_Tree(TRecord&);
	void  delete_BPlus_tree(FILEP, TRecord&);

	void  EnumLeafKey();
	void  SearchRange(TRecord& record, __int64 re, SearchResult& result);
	FILEP GetRoot();
	void WriteRFile(TRecord record,bool flag);
	vector<FILEP> ReadF(SearchResult &result);
	vector<FILEP> ReadF_Only(SearchResult &result, set<FILEP> &st);
	void Select(SearchResult &result, set<FILEP> &st, set<FILEP> &t);
	BPlusTree(string file);
	~BPlusTree();
};

