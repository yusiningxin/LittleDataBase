# include "BPlusTree.h"

BPlusTree::BPlusTree(string file)
{
	string rf = "R" + file;
	Bfile = fopen(file.c_str(), "rb+"); //打开B+树文件，以二进制方式读写，如果不存在则创建一个文件
	
	if (Bfile != NULL)
	{
		_fseeki64(Bfile,0, SEEK_SET);
		fread(&ROOT, sizeof(FILEP), 1, Bfile);

		Rfile = fopen(rf.c_str(), "rb+"); //打开B+树文件，以二进制方式读写，如果不存在则创建一个文件
		FROOT = GetFNode();
		
		cout << ROOT << endl;
	}
	else
	{
		Rfile = fopen(rf.c_str(), "wb+");
		FROOT = GetFNode();
		//保存root位置
		Bfile = fopen(file.c_str(), "wb+");
		ROOT = 0;
		_fseeki64(Bfile, 0, SEEK_SET);
		fwrite(&ROOT, sizeof(FILEP), 1, Bfile);
		ROOT = GetBPlusNode();

		BPlusNode r;
		r.Pointer[MAX_KEY] = 0;
		r.nkey = 0;
		r.isleaf = true;
		WriteBPlusNode(ROOT, r);
	}
	

}

BPlusTree :: ~BPlusTree()
{

	_fseeki64(Bfile, 0, SEEK_SET);
	fwrite(&ROOT, sizeof(FILEP), 1, Bfile);


	_fseeki64(Bfile, 0, SEEK_SET);

	//fclose(Rfile);
}



void BPlusTree::Build_BPlus_Tree()//建立一棵空B+树  
{
	ROOT = GetBPlusNode();
	FROOT = GetFNode();
	BPlusNode r;
	r.Pointer[MAX_KEY] = 0;
	r.nkey = 0;
	r.isleaf = true;
	WriteBPlusNode(ROOT, r);
}
FILEP BPlusTree::GetRoot()
{
	return ROOT;
}


bool    BPlusTree::Insert_BPlus_Tree(TRecord &record)        //向B+树插入关键字  
{
	BPlusNode r;
	ReadBPlusNode(ROOT, r);  //加构造函数

	if (r.nkey == MAX_KEY)
	{
		BPlusNode newroot;
		newroot.nkey = 0;
		newroot.isleaf = false;
		newroot.Pointer[0] = ROOT;

		Split_BPlus_Node(newroot, r, 0);
		WriteBPlusNode(ROOT, r);

		ROOT = GetBPlusNode();

		WriteBPlusNode(ROOT, newroot);

		//分裂根节点  
	}
	if (insert_bplus_tree(ROOT, record))
	{
		return true;
	}
	return false;
}



bool BPlusTree::insert_bplus_tree(FILEP current, TRecord &record)
{
	BPlusNode x;
	ReadBPlusNode(current, x);
	bool be;

	int i;
	for (i = 0; i < x.nkey && x.key[i] < record.key; i++);

	if (i < x.nkey && x.isleaf && x.key[i] == record.key)  //在B+树叶节点找到了相同关键字  
	{
		//cout << record.key << "Key has exited!" << endl;//关键字插入重复
		record.Raddress = x.Pointer[i];
		WriteRFile(record,true); //在倒排链文件中插入新的id
		return false;
	}
	record.Raddress = GetFNode();
	WriteRFile(record, false); //第一次插入该关键字，分配新空间
	if (!x.isleaf)    //如果不是叶节点  
	{
		BPlusNode y;
		ReadBPlusNode(x.Pointer[i], y);

		if (y.nkey == MAX_KEY)     //如果x的子节点已满，则这个子节点分裂  
		{
			Split_BPlus_Node(x, y, i);
			WriteBPlusNode(current, x);
			WriteBPlusNode(x.Pointer[i], y);
		}
		if (record.key <= x.key[i] || i == x.nkey)
		{
			be = insert_bplus_tree(x.Pointer[i], record);
			if (be == false)
				return false;
		}
		else
		{
			be = insert_bplus_tree(x.Pointer[i + 1], record);
			if (be == false)
				return false;

		}

	}
	else          //如果是叶节点,则直接将关键字插入key数组中  
	{

		for (int j = x.nkey; j > i; j--)
		{
			x.key[j] = x.key[j - 1];
			x.Pointer[j] = x.Pointer[j - 1];
		}
		x.key[i] = record.key;
		x.nkey++;

		//将记录的地址赋给x.Pointer[i]  

		x.Pointer[i] = record.Raddress;
		WriteBPlusNode(current, x);

	}
	return true;

}



void    BPlusTree::Split_BPlus_Node(BPlusNode &father, BPlusNode &current, const int childnum)          //分裂满的B+树节点  
{
	int half = MAX_KEY / 2;

	int i;

	for (i = father.nkey; i > childnum; i--)
	{
		father.key[i] = father.key[i - 1];
		father.Pointer[i + 1] = father.Pointer[i];
	}
	father.nkey++;

	BPlusNode t;
	//ReadBPlusNode(ROOT,current);

	FILEP address = GetBPlusNode();

	father.key[childnum] = current.key[half];
	father.Pointer[childnum + 1] = address;

	for (i = half + 1; i < MAX_KEY; i++)
	{
		t.key[i - half - 1] = current.key[i];
		t.Pointer[i - half - 1] = current.Pointer[i];
	}

	t.nkey = MAX_KEY - half - 1;
	t.Pointer[t.nkey] = current.Pointer[MAX_KEY];

	t.isleaf = current.isleaf;

	current.nkey = half;

	if (current.isleaf)   //如果当前被分裂节点是叶子  
	{
		current.nkey++;
		t.Pointer[MAX_KEY] = current.Pointer[MAX_KEY];
		current.Pointer[MAX_KEY] = address;
	}

	WriteBPlusNode(address, t);

}



void BPlusTree::Search_BPlus_Tree(TRecord &record, SearchResult &result, int& p) const        //在B+树查询一个关键字  
{
	int i;

	BPlusNode a;
	FILEP current = ROOT;

	do
	{
		ReadBPlusNode(current, a);

		for (i = 0; i < a.nkey && record.key > a.key[i]; i++);

		p = i;
		result.Baddress = current;
		if (i < a.nkey && a.isleaf && record.key == a.key[i])       //在B+树叶节点找到了等值的关键字  
		{
			result.Baddress = current;
			result.Raddress = a.Pointer[i];
			//返回该关键字所对应的记录的地址  
			result.exist = true;
			return;
		}
		current = a.Pointer[i];
		//	if(!a.isleaf)
		//  result.Raddress = a.Pointer[i];
	} while (!a.isleaf);

	result.exist = false;
}




void    BPlusTree::delete_BPlus_tree(FILEP current, TRecord &record)
{
	int i, j;

	BPlusNode x;
	ReadBPlusNode(current, x);


	for (i = 0; i < x.nkey && record.key > x.key[i]; i++);

	if (i < x.nkey && x.key[i] == record.key)  //在当前节点找到关键字  
	{

		if (!x.isleaf)     //在内节点找到关键字  
		{
			BPlusNode child;
			ReadBPlusNode(x.Pointer[i], child);

			if (child.isleaf)     //如果孩子是叶节点  
			{
				if (child.nkey > MAX_KEY / 2)      //情况A  
				{
					x.key[i] = child.key[child.nkey - 2];
					child.nkey--;

					WriteBPlusNode(current, x);
					WriteBPlusNode(x.Pointer[i], child);

					return;
				}
				else    //否则孩子节点的关键字数量不过半  
				{
					if (i > 0)      //有左兄弟节点  
					{
						BPlusNode lbchild;
						ReadBPlusNode(x.Pointer[i - 1], lbchild);

						if (lbchild.nkey > MAX_KEY / 2)        //情况B  
						{
							for (j = child.nkey; j > 0; j--)
							{
								child.key[j] = child.key[j - 1];
								child.Pointer[j] = child.Pointer[j - 1];
							}

							child.key[0] = x.key[i - 1];
							child.Pointer[0] = lbchild.Pointer[lbchild.nkey - 1];

							child.nkey++;

							lbchild.nkey--;

							x.key[i - 1] = lbchild.key[lbchild.nkey - 1];
							x.key[i] = child.key[child.nkey - 2];

							WriteBPlusNode(current, x);
							WriteBPlusNode(x.Pointer[i - 1], lbchild);
							WriteBPlusNode(x.Pointer[i], child);

						}
						else    //情况C  
						{
							for (j = 0; j < child.nkey; j++)
							{
								lbchild.key[lbchild.nkey + j] = child.key[j];
								lbchild.Pointer[lbchild.nkey + j] = child.Pointer[j];
							}
							lbchild.nkey += child.nkey;

							lbchild.Pointer[MAX_KEY] = child.Pointer[MAX_KEY];

							//释放child节点占用的空间x.Pointer[i]  

							for (j = i - 1; j < x.nkey - 1; j++)
							{
								x.key[j] = x.key[j + 1];
								x.Pointer[j + 1] = x.Pointer[j + 2];
							}
							x.nkey--;

							x.key[i - 1] = lbchild.key[lbchild.nkey - 2];
							WriteBPlusNode(current, x);
							WriteBPlusNode(x.Pointer[i - 1], lbchild);

							i--;

						}


					}
					else      //只有右兄弟节点  
					{
						BPlusNode rbchild;
						ReadBPlusNode(x.Pointer[i + 1], rbchild);

						if (rbchild.nkey > MAX_KEY / 2)        //情况D  
						{
							x.key[i] = rbchild.key[0];
							child.key[child.nkey] = rbchild.key[0];
							child.Pointer[child.nkey] = rbchild.Pointer[0];
							child.nkey++;

							for (j = 0; j < rbchild.nkey - 1; j++)
							{
								rbchild.key[j] = rbchild.key[j + 1];
								rbchild.Pointer[j] = rbchild.Pointer[j + 1];
							}

							rbchild.nkey--;

							WriteBPlusNode(current, x);
							WriteBPlusNode(x.Pointer[i], child);
							WriteBPlusNode(x.Pointer[i + 1], rbchild);

						}
						else    //情况E  
						{
							for (j = 0; j < rbchild.nkey; j++)
							{
								child.key[child.nkey + j] = rbchild.key[j];
								child.Pointer[child.nkey + j] = rbchild.Pointer[j];
							}
							child.nkey += rbchild.nkey;

							child.Pointer[MAX_KEY] = rbchild.Pointer[MAX_KEY];

							//释放rbchild占用的空间x.Pointer[i+1]  

							for (j = i; j < x.nkey - 1; j++)
							{
								x.key[j] = x.key[j + 1];
								x.Pointer[j + 1] = x.Pointer[j + 2];
							}
							x.nkey--;

							WriteBPlusNode(current, x);
							WriteBPlusNode(x.Pointer[i], child);

						}

					}

				}

			}
			else      //情况F  
			{

				//找到key在B+树叶节点的左兄弟关键字,将这个关键字取代key的位置  
				int p0;
				TRecord trecord;
				trecord.key = record.key;
				SearchResult result;
				Search_BPlus_Tree(trecord, result, p0);

				BPlusNode last;

				ReadBPlusNode(result.Baddress, last);

				x.key[i] = last.key[last.nkey - 2];
				WriteBPlusNode(current, x);


				if (child.nkey > MAX_KEY / 2)        //情况H  
				{

				}
				else          //否则孩子节点的关键字数量不过半,则将兄弟节点的某一个关键字移至孩子  
				{
					if (i > 0)  //x.key[i]有左兄弟  
					{
						BPlusNode lbchild;
						ReadBPlusNode(x.Pointer[i - 1], lbchild);

						if (lbchild.nkey > MAX_KEY / 2)       //情况I  
						{
							for (j = child.nkey; j > 0; j--)
							{
								child.key[j] = child.key[j - 1];
								child.Pointer[j + 1] = child.Pointer[j];
							}
							child.Pointer[1] = child.Pointer[0];
							child.key[0] = x.key[i - 1];
							child.Pointer[0] = lbchild.Pointer[lbchild.nkey];

							child.nkey++;

							x.key[i - 1] = lbchild.key[lbchild.nkey - 1];
							lbchild.nkey--;

							WriteBPlusNode(current, x);
							WriteBPlusNode(x.Pointer[i - 1], lbchild);
							WriteBPlusNode(x.Pointer[i], child);
						}
						else        //情况J  
						{
							lbchild.key[lbchild.nkey] = x.key[i - 1];   //将孩子节点复制到其左兄弟的末尾  
							lbchild.nkey++;

							for (j = 0; j < child.nkey; j++)      //将child节点拷贝到lbchild节点的末尾,  
							{
								lbchild.key[lbchild.nkey + j] = child.key[j];
								lbchild.Pointer[lbchild.nkey + j] = child.Pointer[j];
							}
							lbchild.Pointer[lbchild.nkey + j] = child.Pointer[j];
							lbchild.nkey += child.nkey;        //已经将child拷贝到lbchild节点  


							//释放child节点的存储空间,x.Pointer[i]  


							//将找到关键字的孩子child与关键字左兄弟的孩子lbchild合并后,将该关键字前移,使当前节点的关键字减少一个  
							for (j = i - 1; j < x.nkey - 1; j++)
							{
								x.key[j] = x.key[j + 1];
								x.Pointer[j + 1] = x.Pointer[j + 2];
							}
							x.nkey--;

							WriteBPlusNode(current, x);
							WriteBPlusNode(x.Pointer[i - 1], lbchild);

							i--;

						}

					}
					else        //否则x.key[i]只有右兄弟  
					{
						BPlusNode rbchild;
						ReadBPlusNode(x.Pointer[i + 1], rbchild);

						if (rbchild.nkey > MAX_KEY / 2)     //情况K  
						{

							child.key[child.nkey] = x.key[i];
							child.nkey++;

							child.Pointer[child.nkey] = rbchild.Pointer[0];
							x.key[i] = rbchild.key[0];
							for (j = 0; j < rbchild.nkey - 1; j++)
							{
								rbchild.key[j] = rbchild.key[j + 1];
								rbchild.Pointer[j] = rbchild.Pointer[j + 1];
							}
							rbchild.Pointer[j] = rbchild.Pointer[j + 1];
							rbchild.nkey--;

							WriteBPlusNode(current, x);
							WriteBPlusNode(x.Pointer[i], child);
							WriteBPlusNode(x.Pointer[i + 1], rbchild);

						}
						else        //情况L  
						{
							child.key[child.nkey] = x.key[i];
							child.nkey++;

							for (j = 0; j < rbchild.nkey; j++)     //将rbchild节点合并到child节点后  
							{
								child.key[child.nkey + j] = rbchild.key[j];
								child.Pointer[child.nkey + j] = rbchild.Pointer[j];
							}
							child.Pointer[child.nkey + j] = rbchild.Pointer[j];

							child.nkey += rbchild.nkey;

							//释放rbchild节点所占用的空间,x,Pointer[i+1]  

							for (j = i; j < x.nkey - 1; j++)    //当前将关键字之后的关键字左移一位,使该节点的关键字数量减一  
							{
								x.key[j] = x.key[j + 1];
								x.Pointer[j + 1] = x.Pointer[j + 2];
							}
							x.nkey--;

							WriteBPlusNode(current, x);
							WriteBPlusNode(x.Pointer[i], child);

						}

					}
				}

			}

			delete_BPlus_tree(x.Pointer[i], record);

		}
		else  //情况G  
		{
			for (j = i; j < x.nkey - 1; j++)
			{
				x.key[j] = x.key[j + 1];
				x.Pointer[j] = x.Pointer[j + 1];
			}
			x.nkey--;

			WriteBPlusNode(current, x);

			return;
		}

	}
	else        //在当前节点没找到关键字     
	{
		if (!x.isleaf)    //没找到关键字,则关键字必然包含在以Pointer[i]为根的子树中  
		{
			BPlusNode child;
			ReadBPlusNode(x.Pointer[i], child);

			if (!child.isleaf)      //如果其孩子节点是内节点  
			{
				if (child.nkey > MAX_KEY / 2)        //情况H  
				{

				}
				else          //否则孩子节点的关键字数量不过半,则将兄弟节点的某一个关键字移至孩子  
				{
					if (i > 0)  //x.key[i]有左兄弟  
					{
						BPlusNode lbchild;
						ReadBPlusNode(x.Pointer[i - 1], lbchild);

						if (lbchild.nkey > MAX_KEY / 2)       //情况I  
						{
							for (j = child.nkey; j > 0; j--)
							{
								child.key[j] = child.key[j - 1];
								child.Pointer[j + 1] = child.Pointer[j];
							}
							child.Pointer[1] = child.Pointer[0];
							child.key[0] = x.key[i - 1];
							child.Pointer[0] = lbchild.Pointer[lbchild.nkey];

							child.nkey++;

							x.key[i - 1] = lbchild.key[lbchild.nkey - 1];
							lbchild.nkey--;

							WriteBPlusNode(current, x);
							WriteBPlusNode(x.Pointer[i - 1], lbchild);
							WriteBPlusNode(x.Pointer[i], child);
						}
						else        //情况J  
						{
							lbchild.key[lbchild.nkey] = x.key[i - 1];   //将孩子节点复制到其左兄弟的末尾
							lbchild.nkey++;

							for (j = 0; j < child.nkey; j++)      //将child节点拷贝到lbchild节点的末尾,  
							{
								lbchild.key[lbchild.nkey + j] = child.key[j];
								lbchild.Pointer[lbchild.nkey + j] = child.Pointer[j];
							}
							lbchild.Pointer[lbchild.nkey + j] = child.Pointer[j];
							lbchild.nkey += child.nkey;        //已经将child拷贝到lbchild节点  


							//释放child节点的存储空间,x.Pointer[i]  


							//将找到关键字的孩子child与关键字左兄弟的孩子lbchild合并后,将该关键字前移,使当前节点的关键字减少一个  
							for (j = i - 1; j < x.nkey - 1; j++)
							{
								x.key[j] = x.key[j + 1];
								x.Pointer[j + 1] = x.Pointer[j + 2];
							}
							x.nkey--;

							WriteBPlusNode(current, x);
							WriteBPlusNode(x.Pointer[i - 1], lbchild);

							i--;

						}

					}
					else        //否则x.key[i]只有右兄弟  
					{
						BPlusNode rbchild;
						ReadBPlusNode(x.Pointer[i + 1], rbchild);

						if (rbchild.nkey > MAX_KEY / 2)     //情况K  
						{

							child.key[child.nkey] = x.key[i];
							child.nkey++;

							child.Pointer[child.nkey] = rbchild.Pointer[0];
							x.key[i] = rbchild.key[0];
							for (j = 0; j < rbchild.nkey - 1; j++)
							{
								rbchild.key[j] = rbchild.key[j + 1];
								rbchild.Pointer[j] = rbchild.Pointer[j + 1];
							}
							rbchild.Pointer[j] = rbchild.Pointer[j + 1];
							rbchild.nkey--;

							WriteBPlusNode(current, x);
							WriteBPlusNode(x.Pointer[i], child);
							WriteBPlusNode(x.Pointer[i + 1], rbchild);

						}
						else        //情况L  
						{
							child.key[child.nkey] = x.key[i];
							child.nkey++;

							for (j = 0; j < rbchild.nkey; j++)     //将rbchild节点合并到child节点后  
							{
								child.key[child.nkey + j] = rbchild.key[j];
								child.Pointer[child.nkey + j] = rbchild.Pointer[j];
							}
							child.Pointer[child.nkey + j] = rbchild.Pointer[j];

							child.nkey += rbchild.nkey;

							//释放rbchild节点所占用的空间,x,Pointer[i+1]  

							for (j = i; j < x.nkey - 1; j++)    //当前将关键字之后的关键字左移一位,使该节点的关键字数量减一  
							{
								x.key[j] = x.key[j + 1];
								x.Pointer[j + 1] = x.Pointer[j + 2];
							}
							x.nkey--;

							WriteBPlusNode(current, x);
							WriteBPlusNode(x.Pointer[i], child);

						}

					}
				}
			}
			else  //否则其孩子节点是外节点  
			{
				if (child.nkey > MAX_KEY / 2)  //情况M  
				{

				}
				else        //否则孩子节点不到半满  
				{
					if (i > 0) //有左兄弟  
					{
						BPlusNode lbchild;
						ReadBPlusNode(x.Pointer[i - 1], lbchild);

						if (lbchild.nkey > MAX_KEY / 2)       //情况N  
						{
							for (j = child.nkey; j > 0; j--)
							{
								child.key[j] = child.key[j - 1];
								child.Pointer[j] = child.Pointer[j - 1];
							}
							child.key[0] = x.key[i - 1];
							child.Pointer[0] = lbchild.Pointer[lbchild.nkey - 1];
							child.nkey++;
							lbchild.nkey--;

							x.key[i - 1] = lbchild.key[lbchild.nkey - 1];
							WriteBPlusNode(x.Pointer[i - 1], lbchild);
							WriteBPlusNode(x.Pointer[i], child);
							WriteBPlusNode(current, x);

						}
						else        //情况O  
						{

							for (j = 0; j < child.nkey; j++)        //与左兄弟孩子节点合并  
							{
								lbchild.key[lbchild.nkey + j] = child.key[j];
								lbchild.Pointer[lbchild.nkey + j] = child.Pointer[j];
							}
							lbchild.nkey += child.nkey;

							lbchild.Pointer[MAX_KEY] = child.Pointer[MAX_KEY];

							//释放child占用的空间x.Pointer[i]  

							for (j = i - 1; j < x.nkey - 1; j++)
							{
								x.key[j] = x.key[j + 1];
								x.Pointer[j + 1] = x.Pointer[j + 2];
							}

							x.nkey--;

							WriteBPlusNode(x.Pointer[i - 1], lbchild);
							WriteBPlusNode(current, x);

							i--;

						}

					}
					else        //否则只有右兄弟  
					{

						BPlusNode rbchild;
						ReadBPlusNode(x.Pointer[i + 1], rbchild);

						if (rbchild.nkey > MAX_KEY / 2)       //情况P  
						{
							x.key[i] = rbchild.key[0];
							child.key[child.nkey] = rbchild.key[0];
							child.Pointer[child.nkey] = rbchild.Pointer[0];
							child.nkey++;

							for (j = 0; j < rbchild.nkey - 1; j++)
							{
								rbchild.key[j] = rbchild.key[j + 1];
								rbchild.Pointer[j] = rbchild.Pointer[j + 1];
							}
							rbchild.nkey--;

							WriteBPlusNode(current, x);
							WriteBPlusNode(x.Pointer[i + 1], rbchild);
							WriteBPlusNode(x.Pointer[i], child);

						}
						else        //情况Q  
						{
							for (j = 0; j < rbchild.nkey; j++)
							{
								child.key[child.nkey + j] = rbchild.key[j];
								child.Pointer[child.nkey + j] = rbchild.Pointer[j];
							}
							child.nkey += rbchild.nkey;
							child.Pointer[MAX_KEY] = rbchild.Pointer[MAX_KEY];

							//释放rbchild占用的空间x.Pointer[i+1]  

							for (j = i; j < x.nkey - 1; j++)
							{
								x.key[j] = x.key[j + 1];
								x.Pointer[j + 1] = x.Pointer[j + 2];
							}
							x.nkey--;

							WriteBPlusNode(current, x);
							WriteBPlusNode(x.Pointer[i], child);


						}

					}

				}

			}

			delete_BPlus_tree(x.Pointer[i], record);
		}


	}


}



void    BPlusTree::Delete_BPlus_Tree(TRecord &record)    //在B+中删除一个关键字  
{
	delete_BPlus_tree(ROOT, record);

	BPlusNode rootnode;
	ReadBPlusNode(ROOT, rootnode);

	if (!rootnode.isleaf && rootnode.nkey == 0)    //如果删除关键字后根节点不是叶节点，并且关键字数量为0时根节点也应该被删除  
	{
		//释放ROOT节点占用的空间  
		ROOT = rootnode.Pointer[0];         //根节点下移,B+树高度减1  

	}

}




void  BPlusTree::EnumLeafKey()    //依次枚举B+树叶节点的所有关键字  
{
	BPlusNode head;

	ReadBPlusNode(ROOT, head);
	//cout << ROOT << endl;
	while (!head.isleaf)
	{
		ReadBPlusNode(head.Pointer[0], head);
	}
	while (1)
	{
		for (int i = 0; i < head.nkey; i++)
		{
			printf("关键字为 %I64u   ", head.key[i]);
			printf("指向的地址为 %lu ", head.Pointer[i]);
			cout << endl;
		}
		
		if (head.Pointer[MAX_KEY] == 0)
			break;

		ReadBPlusNode(head.Pointer[MAX_KEY], head);
	}
}
void BPlusTree::SearchRange(TRecord& record, __int64 re, SearchResult& result)
{
	BPlusNode head;
	int p = 0;
	Search_BPlus_Tree(record, result, p);
	ReadBPlusNode(result.Baddress, head); //1300
	int k = 0;
	int i;
	while (1)
	{

		if (k == 1)
		{
			i = 0;
		}
		if (k == 0 && !result.exist)
		{
			i = p;
			k = 1;
		}
		if (k == 0 && result.exist)
		{
			i = p + 1;
			k = 1;
		}
		for (; i < head.nkey; i++)
		{

			if (head.key[i] >= re)
			{
				k = 2;
				break;
			}
			printf("telephone number is%I64u\n", head.key[i]);
		}
		if (k == 2)
			break;
		if (head.Pointer[MAX_KEY] == 0)
			break;
		ReadBPlusNode(head.Pointer[MAX_KEY], head);
	}
	cout << endl;


}





inline FILEP BPlusTree::GetBPlusNode()  const //在磁盘上分配一块B+树节点空间  
{
	_fseeki64(Bfile, 0, SEEK_END);

	return  _ftelli64(Bfile);
}

inline FILEP BPlusTree::GetFNode()  const //在磁盘上新分配一块倒排链空间  
{
	_fseeki64(Rfile, 0, SEEK_END);

	return  _ftelli64(Rfile);
}

vector<FILEP> BPlusTree::ReadF(SearchResult &result)
{
	vector<FILEP> vec;
	_fseeki64(Rfile, result.Raddress, SEEK_SET);
	FILEP *tmp;
	int n = 5,cnt=1;
	tmp = new FILEP[n];
	while (fread(tmp, sizeof(FILEP)*n, 1, Rfile))
	{
		if (cnt==1)
		{
			result.idNum = tmp[0];
			for (int i = 2; i < n - 1; i++)
			{
				if (tmp[i] == 0) break;
				//cout << tmp[i] << "-";
				vec.push_back(tmp[i]);
			}
		}
		else
		{
			for (int i = 0; i < n -1; i++)
			{
				if (tmp[i] == 0) break;
				//cout << tmp[i] << "-";
				vec.push_back(tmp[i]);
			}
		}
		if (tmp[n - 1] == 0) break;
		_fseeki64(Rfile, tmp[n-1], SEEK_SET);
		cnt++;
		n = (1 << cnt) + 1;
		delete[]tmp;
		tmp = new FILEP[n];
	}
	delete[]tmp;
	return vec;
}

vector<FILEP> BPlusTree::ReadF_Only(SearchResult &result, set<FILEP> &st)
{
	vector<FILEP> vec;
	_fseeki64(Rfile, result.Raddress, SEEK_SET);
	FILEP *tmp;
	int n = 5, cnt = 1;
	tmp = new FILEP[n];
	while (fread(tmp, sizeof(FILEP)*n, 1, Rfile))
	{
		if (cnt == 1)
		{
			result.idNum = tmp[0];
			for (int i = 2; i < n - 1; i++)
			{
				if (tmp[i] == 0) break;
				if(st.find(tmp[i])==st.end()) vec.push_back(tmp[i]);
			}
		}
		else
		{
			for (int i = 0; i < n - 1; i++)
			{
				if (tmp[i] == 0) break;
				if (st.find(tmp[i]) == st.end()) vec.push_back(tmp[i]);
			}
		}
		if (tmp[n - 1] == 0) break;
		_fseeki64(Rfile, tmp[n - 1], SEEK_SET);
		cnt++;
		n = (1 << cnt) + 1;
		delete[]tmp;
		tmp = new FILEP[n];
	}
	delete[]tmp;
	return vec;
}
void BPlusTree::Select(SearchResult &result, set<FILEP> &st,set<FILEP> &t)
{
	_fseeki64(Rfile, result.Raddress, SEEK_SET);
	FILEP *tmp;
	int n = 5, cnt = 1;
	tmp = new FILEP[n];
	while (fread(tmp, sizeof(FILEP)*n, 1, Rfile))
	{
		if (cnt == 1)
		{
			for (int i = 2; i < n - 1; i++)
			{
				if (tmp[i] == 0) break;
				if (t.find(tmp[i]) == t.end()) st.insert(tmp[i]);
			}
		}
		else
		{
			for (int i = 0; i < n - 1; i++)
			{
				if (tmp[i] == 0) break;
				if (t.find(tmp[i]) == t.end()) st.insert(tmp[i]);
			}
		}
		if (tmp[n - 1] == 0) break;
		_fseeki64(Rfile, tmp[n - 1], SEEK_SET);
		cnt++;
		n = (1 << cnt) + 1;
		delete[]tmp;
		tmp = new FILEP[n];
	}
	delete[]tmp;
}


inline void BPlusTree::ReadBPlusNode(const FILEP address, BPlusNode   &r) const //读取address地址上的一块B+树节点  
{
	_fseeki64(Bfile, address, SEEK_SET);
	fread((&r), sizeof(BPlusNode), 1, Bfile);
	//(char*)(&r)
}

inline void BPlusTree::WriteRFile(TRecord record, bool flag) //将一个B+树节点写入address地址  
{
	if (!flag) //第一次分配空间
	{
		FILEP F[5];
		memset(F, 0, sizeof(F));
		F[0] = 1;
		F[1] = record.Raddress;
		F[2] = record.id;
		_fseeki64(Rfile, record.Raddress, SEEK_SET);
		fwrite(&F, sizeof(FILEP)*5, 1, Rfile);
	}
	else 
	{
		FILEP sum = 0,write;
		int n=5,cnt=0;
		FILEP tmp[5];
		_fseeki64(Rfile, record.Raddress, SEEK_SET);
		fread(tmp, sizeof(FILEP)*n, 1, Rfile);
		do
		{
			cnt++;
			sum += (1 << cnt);
		} while (tmp[0]>sum);
		if (cnt == 1) write = record.Raddress+sizeof(FILEP)*4;
		else write = tmp[1] + sizeof(FILEP)*(1 << cnt);

		if (tmp[0]==sum) //需要分配新的块
		{
			//添加关键字的id
			FILEP addr = GetFNode();
			n = (1 << cnt + 1)+1;
			FILEP *t = new FILEP[n];
			memset(t, 0,sizeof(FILEP)*n);
			t[0] = record.id;
			t[n-1] = 0;
			fwrite(t, sizeof(FILEP) * n, 1, Rfile);
			delete []t;
			//修改id总数以及最后一次分配空间的开始地址
			tmp[0]++;
			tmp[1] = addr;
			_fseeki64(Rfile, record.Raddress, SEEK_SET);
			fwrite(tmp, sizeof(FILEP) * 2, 1, Rfile);

			//将addr地址填写到相地址中以便后续遍历
			/*n = 5,cnt=1;
			FILEP write = record.Raddress;
			while (tmp[n - 1] != 0)
			{
				_fseeki64(Rfile, tmp[n-1], SEEK_SET);
				write = tmp[n - 1];
				cnt++;
				n = (1 << cnt) + 1; 
				tmp = new FILEP(n);
				fread(tmp, sizeof(FILEP)*n, 1, Rfile);
			}

			tmp[n - 1] = addr;
			_fseeki64(Rfile, write + sizeof(FILEP)*(n - 1), SEEK_SET);
			fwrite(&tmp[n-1], sizeof(FILEP), 1, Rfile);	*/
			tmp[0] = addr;
			_fseeki64(Rfile, write, SEEK_SET);
			fwrite(&tmp[0], sizeof(FILEP), 1, Rfile);
		}
		else //无需分配新块直接在空白处添加id
		{
			tmp[0]++;
			_fseeki64(Rfile, record.Raddress, SEEK_SET);
			fwrite(tmp, sizeof(FILEP), 1, Rfile);
			sum -= (1 << cnt);
			n = tmp[0] - sum-1;
			if (sum == 0) //第一次分配处添加
			{
				_fseeki64(Rfile, tmp[1] + sizeof(FILEP)*(n+2), SEEK_SET);
				fwrite(&record.id, sizeof(FILEP), 1, Rfile);
			}
			else
			{
				_fseeki64(Rfile, tmp[1] + sizeof(FILEP)*n, SEEK_SET);
				fwrite(&record.id, sizeof(FILEP), 1, Rfile);
			}
		
		}
	}
}

inline void BPlusTree::WriteBPlusNode(const FILEP address, const BPlusNode &r) //将一个B+树节点写入address地址  
{
	_fseeki64(Bfile, address, SEEK_SET);
	fwrite((&r), sizeof(BPlusNode), 1, Bfile);
}