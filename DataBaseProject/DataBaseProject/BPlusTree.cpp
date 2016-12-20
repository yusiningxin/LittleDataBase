# include "BPlusTree.h"

BPlusTree::BPlusTree(string file)
{
	string rf = "R" + file;
	Bfile = fopen(file.c_str(), "rb+"); //��B+���ļ����Զ����Ʒ�ʽ��д������������򴴽�һ���ļ�
	
	if (Bfile != NULL)
	{
		_fseeki64(Bfile,0, SEEK_SET);
		fread(&ROOT, sizeof(FILEP), 1, Bfile);

		Rfile = fopen(rf.c_str(), "rb+"); //��B+���ļ����Զ����Ʒ�ʽ��д������������򴴽�һ���ļ�
		FROOT = GetFNode();
		
		cout << ROOT << endl;
	}
	else
	{
		Rfile = fopen(rf.c_str(), "wb+");
		FROOT = GetFNode();
		//����rootλ��
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



void BPlusTree::Build_BPlus_Tree()//����һ�ÿ�B+��  
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


bool    BPlusTree::Insert_BPlus_Tree(TRecord &record)        //��B+������ؼ���  
{
	BPlusNode r;
	ReadBPlusNode(ROOT, r);  //�ӹ��캯��

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

		//���Ѹ��ڵ�  
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

	if (i < x.nkey && x.isleaf && x.key[i] == record.key)  //��B+��Ҷ�ڵ��ҵ�����ͬ�ؼ���  
	{
		//cout << record.key << "Key has exited!" << endl;//�ؼ��ֲ����ظ�
		record.Raddress = x.Pointer[i];
		WriteRFile(record,true); //�ڵ������ļ��в����µ�id
		return false;
	}
	record.Raddress = GetFNode();
	WriteRFile(record, false); //��һ�β���ùؼ��֣������¿ռ�
	if (!x.isleaf)    //�������Ҷ�ڵ�  
	{
		BPlusNode y;
		ReadBPlusNode(x.Pointer[i], y);

		if (y.nkey == MAX_KEY)     //���x���ӽڵ�������������ӽڵ����  
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
	else          //�����Ҷ�ڵ�,��ֱ�ӽ��ؼ��ֲ���key������  
	{

		for (int j = x.nkey; j > i; j--)
		{
			x.key[j] = x.key[j - 1];
			x.Pointer[j] = x.Pointer[j - 1];
		}
		x.key[i] = record.key;
		x.nkey++;

		//����¼�ĵ�ַ����x.Pointer[i]  

		x.Pointer[i] = record.Raddress;
		WriteBPlusNode(current, x);

	}
	return true;

}



void    BPlusTree::Split_BPlus_Node(BPlusNode &father, BPlusNode &current, const int childnum)          //��������B+���ڵ�  
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

	if (current.isleaf)   //�����ǰ�����ѽڵ���Ҷ��  
	{
		current.nkey++;
		t.Pointer[MAX_KEY] = current.Pointer[MAX_KEY];
		current.Pointer[MAX_KEY] = address;
	}

	WriteBPlusNode(address, t);

}



void BPlusTree::Search_BPlus_Tree(TRecord &record, SearchResult &result, int& p) const        //��B+����ѯһ���ؼ���  
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
		if (i < a.nkey && a.isleaf && record.key == a.key[i])       //��B+��Ҷ�ڵ��ҵ��˵�ֵ�Ĺؼ���  
		{
			result.Baddress = current;
			result.Raddress = a.Pointer[i];
			//���ظùؼ�������Ӧ�ļ�¼�ĵ�ַ  
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

	if (i < x.nkey && x.key[i] == record.key)  //�ڵ�ǰ�ڵ��ҵ��ؼ���  
	{

		if (!x.isleaf)     //���ڽڵ��ҵ��ؼ���  
		{
			BPlusNode child;
			ReadBPlusNode(x.Pointer[i], child);

			if (child.isleaf)     //���������Ҷ�ڵ�  
			{
				if (child.nkey > MAX_KEY / 2)      //���A  
				{
					x.key[i] = child.key[child.nkey - 2];
					child.nkey--;

					WriteBPlusNode(current, x);
					WriteBPlusNode(x.Pointer[i], child);

					return;
				}
				else    //�����ӽڵ�Ĺؼ�������������  
				{
					if (i > 0)      //�����ֵܽڵ�  
					{
						BPlusNode lbchild;
						ReadBPlusNode(x.Pointer[i - 1], lbchild);

						if (lbchild.nkey > MAX_KEY / 2)        //���B  
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
						else    //���C  
						{
							for (j = 0; j < child.nkey; j++)
							{
								lbchild.key[lbchild.nkey + j] = child.key[j];
								lbchild.Pointer[lbchild.nkey + j] = child.Pointer[j];
							}
							lbchild.nkey += child.nkey;

							lbchild.Pointer[MAX_KEY] = child.Pointer[MAX_KEY];

							//�ͷ�child�ڵ�ռ�õĿռ�x.Pointer[i]  

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
					else      //ֻ�����ֵܽڵ�  
					{
						BPlusNode rbchild;
						ReadBPlusNode(x.Pointer[i + 1], rbchild);

						if (rbchild.nkey > MAX_KEY / 2)        //���D  
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
						else    //���E  
						{
							for (j = 0; j < rbchild.nkey; j++)
							{
								child.key[child.nkey + j] = rbchild.key[j];
								child.Pointer[child.nkey + j] = rbchild.Pointer[j];
							}
							child.nkey += rbchild.nkey;

							child.Pointer[MAX_KEY] = rbchild.Pointer[MAX_KEY];

							//�ͷ�rbchildռ�õĿռ�x.Pointer[i+1]  

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
			else      //���F  
			{

				//�ҵ�key��B+��Ҷ�ڵ�����ֵܹؼ���,������ؼ���ȡ��key��λ��  
				int p0;
				TRecord trecord;
				trecord.key = record.key;
				SearchResult result;
				Search_BPlus_Tree(trecord, result, p0);

				BPlusNode last;

				ReadBPlusNode(result.Baddress, last);

				x.key[i] = last.key[last.nkey - 2];
				WriteBPlusNode(current, x);


				if (child.nkey > MAX_KEY / 2)        //���H  
				{

				}
				else          //�����ӽڵ�Ĺؼ�������������,���ֵܽڵ��ĳһ���ؼ�����������  
				{
					if (i > 0)  //x.key[i]�����ֵ�  
					{
						BPlusNode lbchild;
						ReadBPlusNode(x.Pointer[i - 1], lbchild);

						if (lbchild.nkey > MAX_KEY / 2)       //���I  
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
						else        //���J  
						{
							lbchild.key[lbchild.nkey] = x.key[i - 1];   //�����ӽڵ㸴�Ƶ������ֵܵ�ĩβ  
							lbchild.nkey++;

							for (j = 0; j < child.nkey; j++)      //��child�ڵ㿽����lbchild�ڵ��ĩβ,  
							{
								lbchild.key[lbchild.nkey + j] = child.key[j];
								lbchild.Pointer[lbchild.nkey + j] = child.Pointer[j];
							}
							lbchild.Pointer[lbchild.nkey + j] = child.Pointer[j];
							lbchild.nkey += child.nkey;        //�Ѿ���child������lbchild�ڵ�  


							//�ͷ�child�ڵ�Ĵ洢�ռ�,x.Pointer[i]  


							//���ҵ��ؼ��ֵĺ���child��ؼ������ֵܵĺ���lbchild�ϲ���,���ùؼ���ǰ��,ʹ��ǰ�ڵ�Ĺؼ��ּ���һ��  
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
					else        //����x.key[i]ֻ�����ֵ�  
					{
						BPlusNode rbchild;
						ReadBPlusNode(x.Pointer[i + 1], rbchild);

						if (rbchild.nkey > MAX_KEY / 2)     //���K  
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
						else        //���L  
						{
							child.key[child.nkey] = x.key[i];
							child.nkey++;

							for (j = 0; j < rbchild.nkey; j++)     //��rbchild�ڵ�ϲ���child�ڵ��  
							{
								child.key[child.nkey + j] = rbchild.key[j];
								child.Pointer[child.nkey + j] = rbchild.Pointer[j];
							}
							child.Pointer[child.nkey + j] = rbchild.Pointer[j];

							child.nkey += rbchild.nkey;

							//�ͷ�rbchild�ڵ���ռ�õĿռ�,x,Pointer[i+1]  

							for (j = i; j < x.nkey - 1; j++)    //��ǰ���ؼ���֮��Ĺؼ�������һλ,ʹ�ýڵ�Ĺؼ���������һ  
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
		else  //���G  
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
	else        //�ڵ�ǰ�ڵ�û�ҵ��ؼ���     
	{
		if (!x.isleaf)    //û�ҵ��ؼ���,��ؼ��ֱ�Ȼ��������Pointer[i]Ϊ����������  
		{
			BPlusNode child;
			ReadBPlusNode(x.Pointer[i], child);

			if (!child.isleaf)      //����亢�ӽڵ����ڽڵ�  
			{
				if (child.nkey > MAX_KEY / 2)        //���H  
				{

				}
				else          //�����ӽڵ�Ĺؼ�������������,���ֵܽڵ��ĳһ���ؼ�����������  
				{
					if (i > 0)  //x.key[i]�����ֵ�  
					{
						BPlusNode lbchild;
						ReadBPlusNode(x.Pointer[i - 1], lbchild);

						if (lbchild.nkey > MAX_KEY / 2)       //���I  
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
						else        //���J  
						{
							lbchild.key[lbchild.nkey] = x.key[i - 1];   //�����ӽڵ㸴�Ƶ������ֵܵ�ĩβ
							lbchild.nkey++;

							for (j = 0; j < child.nkey; j++)      //��child�ڵ㿽����lbchild�ڵ��ĩβ,  
							{
								lbchild.key[lbchild.nkey + j] = child.key[j];
								lbchild.Pointer[lbchild.nkey + j] = child.Pointer[j];
							}
							lbchild.Pointer[lbchild.nkey + j] = child.Pointer[j];
							lbchild.nkey += child.nkey;        //�Ѿ���child������lbchild�ڵ�  


							//�ͷ�child�ڵ�Ĵ洢�ռ�,x.Pointer[i]  


							//���ҵ��ؼ��ֵĺ���child��ؼ������ֵܵĺ���lbchild�ϲ���,���ùؼ���ǰ��,ʹ��ǰ�ڵ�Ĺؼ��ּ���һ��  
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
					else        //����x.key[i]ֻ�����ֵ�  
					{
						BPlusNode rbchild;
						ReadBPlusNode(x.Pointer[i + 1], rbchild);

						if (rbchild.nkey > MAX_KEY / 2)     //���K  
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
						else        //���L  
						{
							child.key[child.nkey] = x.key[i];
							child.nkey++;

							for (j = 0; j < rbchild.nkey; j++)     //��rbchild�ڵ�ϲ���child�ڵ��  
							{
								child.key[child.nkey + j] = rbchild.key[j];
								child.Pointer[child.nkey + j] = rbchild.Pointer[j];
							}
							child.Pointer[child.nkey + j] = rbchild.Pointer[j];

							child.nkey += rbchild.nkey;

							//�ͷ�rbchild�ڵ���ռ�õĿռ�,x,Pointer[i+1]  

							for (j = i; j < x.nkey - 1; j++)    //��ǰ���ؼ���֮��Ĺؼ�������һλ,ʹ�ýڵ�Ĺؼ���������һ  
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
			else  //�����亢�ӽڵ�����ڵ�  
			{
				if (child.nkey > MAX_KEY / 2)  //���M  
				{

				}
				else        //�����ӽڵ㲻������  
				{
					if (i > 0) //�����ֵ�  
					{
						BPlusNode lbchild;
						ReadBPlusNode(x.Pointer[i - 1], lbchild);

						if (lbchild.nkey > MAX_KEY / 2)       //���N  
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
						else        //���O  
						{

							for (j = 0; j < child.nkey; j++)        //�����ֵܺ��ӽڵ�ϲ�  
							{
								lbchild.key[lbchild.nkey + j] = child.key[j];
								lbchild.Pointer[lbchild.nkey + j] = child.Pointer[j];
							}
							lbchild.nkey += child.nkey;

							lbchild.Pointer[MAX_KEY] = child.Pointer[MAX_KEY];

							//�ͷ�childռ�õĿռ�x.Pointer[i]  

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
					else        //����ֻ�����ֵ�  
					{

						BPlusNode rbchild;
						ReadBPlusNode(x.Pointer[i + 1], rbchild);

						if (rbchild.nkey > MAX_KEY / 2)       //���P  
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
						else        //���Q  
						{
							for (j = 0; j < rbchild.nkey; j++)
							{
								child.key[child.nkey + j] = rbchild.key[j];
								child.Pointer[child.nkey + j] = rbchild.Pointer[j];
							}
							child.nkey += rbchild.nkey;
							child.Pointer[MAX_KEY] = rbchild.Pointer[MAX_KEY];

							//�ͷ�rbchildռ�õĿռ�x.Pointer[i+1]  

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



void    BPlusTree::Delete_BPlus_Tree(TRecord &record)    //��B+��ɾ��һ���ؼ���  
{
	delete_BPlus_tree(ROOT, record);

	BPlusNode rootnode;
	ReadBPlusNode(ROOT, rootnode);

	if (!rootnode.isleaf && rootnode.nkey == 0)    //���ɾ���ؼ��ֺ���ڵ㲻��Ҷ�ڵ㣬���ҹؼ�������Ϊ0ʱ���ڵ�ҲӦ�ñ�ɾ��  
	{
		//�ͷ�ROOT�ڵ�ռ�õĿռ�  
		ROOT = rootnode.Pointer[0];         //���ڵ�����,B+���߶ȼ�1  

	}

}




void  BPlusTree::EnumLeafKey()    //����ö��B+��Ҷ�ڵ�����йؼ���  
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
			printf("�ؼ���Ϊ %I64u   ", head.key[i]);
			printf("ָ��ĵ�ַΪ %lu ", head.Pointer[i]);
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





inline FILEP BPlusTree::GetBPlusNode()  const //�ڴ����Ϸ���һ��B+���ڵ�ռ�  
{
	_fseeki64(Bfile, 0, SEEK_END);

	return  _ftelli64(Bfile);
}

inline FILEP BPlusTree::GetFNode()  const //�ڴ������·���һ�鵹�����ռ�  
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


inline void BPlusTree::ReadBPlusNode(const FILEP address, BPlusNode   &r) const //��ȡaddress��ַ�ϵ�һ��B+���ڵ�  
{
	_fseeki64(Bfile, address, SEEK_SET);
	fread((&r), sizeof(BPlusNode), 1, Bfile);
	//(char*)(&r)
}

inline void BPlusTree::WriteRFile(TRecord record, bool flag) //��һ��B+���ڵ�д��address��ַ  
{
	if (!flag) //��һ�η���ռ�
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

		if (tmp[0]==sum) //��Ҫ�����µĿ�
		{
			//��ӹؼ��ֵ�id
			FILEP addr = GetFNode();
			n = (1 << cnt + 1)+1;
			FILEP *t = new FILEP[n];
			memset(t, 0,sizeof(FILEP)*n);
			t[0] = record.id;
			t[n-1] = 0;
			fwrite(t, sizeof(FILEP) * n, 1, Rfile);
			delete []t;
			//�޸�id�����Լ����һ�η���ռ�Ŀ�ʼ��ַ
			tmp[0]++;
			tmp[1] = addr;
			_fseeki64(Rfile, record.Raddress, SEEK_SET);
			fwrite(tmp, sizeof(FILEP) * 2, 1, Rfile);

			//��addr��ַ��д�����ַ���Ա��������
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
		else //��������¿�ֱ���ڿհ״����id
		{
			tmp[0]++;
			_fseeki64(Rfile, record.Raddress, SEEK_SET);
			fwrite(tmp, sizeof(FILEP), 1, Rfile);
			sum -= (1 << cnt);
			n = tmp[0] - sum-1;
			if (sum == 0) //��һ�η��䴦���
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

inline void BPlusTree::WriteBPlusNode(const FILEP address, const BPlusNode &r) //��һ��B+���ڵ�д��address��ַ  
{
	_fseeki64(Bfile, address, SEEK_SET);
	fwrite((&r), sizeof(BPlusNode), 1, Bfile);
}