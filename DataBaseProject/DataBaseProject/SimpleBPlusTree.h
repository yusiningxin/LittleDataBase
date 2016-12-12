#include<iostream>  
#include<time.h> 
#include<cstdlib> 
#include<string>
#include<vector>
#include"BPlusTree.h"
using namespace std;


class SimpleBPlusTree
{
public:
	FILEP ROOT;       //�������ļ��ڵ�ƫ�Ƶ�ַ  
	FILE  *Bfile;     //B+���ļ���ָ��    
	KEYTYPE idSum;

public:

	FILEP GetBPlusNode() const;

	void  ReadBPlusNode(const FILEP, BPlusNode&) const;
	void  WriteBPlusNode(const FILEP, const BPlusNode&);


	bool  Insert_BPlus_Tree(TRecord&);
	bool  insert_bplus_tree(FILEP, TRecord&);

	void  Split_BPlus_Node(BPlusNode&, BPlusNode&, const int);

	void  Search_BPlus_Tree(TRecord&, SearchResult&, int&) const;

	void  Delete_BPlus_Tree(TRecord&);
	void  delete_BPlus_tree(FILEP, TRecord&);

	void  EnumLeafKey();
	
	FILEP GetRoot();

	SimpleBPlusTree(string file);
	~SimpleBPlusTree();
};

