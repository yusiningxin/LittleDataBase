#include<iostream>  
#include<time.h> 
#include<cstdlib> 
#include<string>
#include<vector>
#include<set>
using namespace std;


#define MAX_KEY 255   //B+树的阶,必须为大于3奇数  
typedef  __int64  KEYTYPE;
typedef  __int64  FILEP;
//B+树节点的数据结构 

struct  BPlusNode
{
	KEYTYPE key[MAX_KEY];      //关键字域  
	FILEP Pointer[MAX_KEY + 1];    //指针域  
	int   nkey;              //关键字数  
	bool  isleaf;                //是否为叶节点 叶节点:true 否则为false  
};

//插入关键字的数据结构  
struct  TRecord
{
	KEYTYPE   key; //该记录的关键字
	FILEP Raddress;         //该关键字对应记录的地址
	FILEP id;
};

//保存查询结果的数据结构  
struct    SearchResult
{
	bool  exist;
	FILEP Baddress;   //保存包含该记录的B+树节点地址  
	FILEP Raddress;   //该关键字的所指向的记录地址
	FILEP idNum;
};

class BPlusTree
{

	FILEP ROOT;       //树根在文件内的偏移地址  
	FILEP FROOT;
	FILE  *Bfile;     //B+树文件的指针  
	FILE  *Rfile;     //记录文件的指针  

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

