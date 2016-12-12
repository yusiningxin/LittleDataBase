# include "iostream"  
# include "string"  
# include "vector" 
# include "atlstr.h"
using namespace std;
int num[6] = { 0, 7, 7, 15, 15, 15 };
int MOD[6] = { 0, 127, 127, 32767, 32767, 32767 };
typedef  __int64  KEYTYPE;
unsigned int SDBMHash(char *str)
{
	unsigned int hash = 0;
	while (*str)
	{
		hash = (*str++) + (hash << 6) + (hash << 16) - hash;
	}
	return (hash & 0x7FFFFFFF);
}

unsigned int RSHash(char *str)
{
	unsigned int b = 378551;
	unsigned int a = 63689;
	unsigned int hash = 0;
	while (*str)
	{
		hash = hash * a + (*str++);
		a *= b;
	}
	return (hash & 0x7FFFFFFF);
}

unsigned int JSHash(char *str)
{
	unsigned int hash = 1315423911;

	while (*str)
	{
		hash ^= ((hash << 5) + (*str++) + (hash >> 2));
	}

	return (hash & 0x7FFFFFFF);
}

KEYTYPE HashMap(string str1, string str2)
{
	unsigned int tmp[6];
	tmp[1] = SDBMHash((char *)str1.c_str());
	tmp[2] = RSHash((char *)str1.c_str());
	tmp[3] = SDBMHash((char *)str2.c_str());
	tmp[4] = RSHash((char *)str2.c_str());
	tmp[5] = JSHash((char *)str2.c_str());
	KEYTYPE ans = 0;
	for (int i = 1; i <= 5; i++)
	{
		ans = ans << num[i];
		tmp[i] = tmp[i] % MOD[i];
		ans += tmp[i];
	}
	return ans;
}