#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include<time.h>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <queue>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <stdio.h>
#include <deque>
#include <map>
#include <set>
#include <string>
#define ll long long
typedef long long LL;
typedef unsigned long long ULL;
//typedef long double ld;
using namespace std;
const int MAXN = 1e6 + 50;
const long double eps = (1e-12);
const int logn = 21;
const double Pi = acos(-1.0);
const LL pp = 1000000000 + 7;
int lowbit(int x) { return x & (-x); }
#define MP(i,j) make_pair(i,j)
#define M(x) ((((x)%pp)+pp)%pp)
//#define int LL
const int BLOCK = sqrt(MAXN);
#define P pair<int,int>
const int INF = 1e9;

/*
加密解密
实例化Encry enc;
调用enc.work_encry(char * addr_source, char * addr_target, int code, int op)
其中addr_source为源文件地址、addr_target为目标文件地址、code为密码、op为操作类型，0为加密1为解密
OrangeRain
*/
class Encry {
private:
	char adds[1000];
	char addt[1000];
	int code;

	vector<char> output;

public:
	void work_encry(char* adds, char* addt, int code,int op) {//code是密码,op=0是加密，op=1是解密
		freopen(adds, "r", stdin);
		freopen(addt, "w", stdout);
		output.clear();
		char c = getchar();
		srand(code);
		while (c != EOF) {
			if (c == -2) {
				c = 26;
			}
			int now = rand() % 128;//不能搞出-1
			char d = c ^ now;
			if (d < 0) {
				cout << "?";
			}
			output.push_back(d);
			c = getchar();
		}
		for (auto a : output) {
			if (a == 26) {//这里有神秘bug
				a = -2;
			}
			printf("%c", a);
		}
		fclose(stdin);
		fclose(stdout);
		//freopen("CON", "r", stdin);
		//freopen("CON", "w", stdout);
	}
};

/*
压缩
实例化Code co;
调用co.work_code(char * addr_source, char * addr_target)
其中addr_source为源文件地址、addr_target为目标文件地址
OrangeRain
*/
class Code {
private:
	char adds[1000];
	char addt[1000];
	int belongs[1000];//字符i属于belongs[i]对应的id
	int tong[140];
	int root = -1;
	struct node {
		int id; int value;
		int p; int l; int r;
		bool operator < (const struct node& v) const {
			if (value == v.value) return id < v.id;
			return value < v.value;
		}
	};
	node tree[1000];//从1开始
	int cnt = 0;
	set<node> st;
	vector<char> table[130];//暴力建表

	vector<char> text;
	vector<char> zip;
	vector<char> output;
	int sup0 = 0;

	void dfs(int x) {
		int u = belongs[x];
		stack<int> tmp;
		while (u != root) {
			int p = tree[u].p;
			if (tree[p].l == u) {//左0右1
				tmp.push(0);
			}
			else {
				tmp.push(1);
			}
			u = p;
		}
		while (!tmp.empty()) {
			table[x].push_back(tmp.top());
			tmp.pop();
		}
	}
public:
	void work_code(char* adds, char* addt) {
		freopen(adds, "r", stdin);
		freopen(addt, "w", stdout);
		char c = getchar();
		for (int i = 0; i <= 127; i++) {
			tong[i] = 0;
		}
		while (c != EOF) {
			if (c == -2) c = 26;//..
			text.push_back(c);
			tong[c]++;
			c = getchar();
		}
		for (int i = 0; i <= 127; i++) {
			if (tong[i]) {
				cnt++; belongs[i] = cnt;
				tree[cnt].value = tong[i]; tree[cnt].id = cnt; tree[cnt].l = tree[cnt].r = -1;
				st.insert(tree[cnt]);
			}
		}
		while (st.size() > 1) {
			node a = *st.begin(); st.erase(st.begin());
			node b = *st.begin(); st.erase(st.begin());
			cnt++; tree[cnt].value = a.value + b.value;
			tree[cnt].l = a.id; tree[cnt].r = b.id; tree[cnt].id = cnt;

			tree[a.id].p = cnt; tree[b.id].p = cnt;
			st.insert(tree[cnt]);
		}
		root = cnt;

		for (int i = 0; i <= 127; i++) {
			if (tong[i]) {
				dfs(i);
			}
		}
		for (auto a : text) {
			for (auto x : table[a]) {
				zip.push_back(x);
			}
		}
		while (zip.size() % 7 != 0) {
			zip.push_back(0); sup0++;
		}
		stack<char> tmp;
		while (!zip.empty()) {
			int base = 1;   char ans = 0;
			for (int i = 0; i < 7; i++) {
				ans += base * zip[zip.size() - 1];
				base *= 2;
				zip.pop_back();
			}
			tmp.push(ans);
		}
		while (!tmp.empty()) {
			output.push_back(tmp.top()); tmp.pop();
		}
		printf("%d\n", sup0);//输出补了多少0
		for (int i = 0; i <= 127; i++) {
			printf("%d ", i);
			for (auto a : table[i]) {
				printf("%c", a + '0');//便于阅读
			}
			if (table[i].size() == 0) {
				printf("-1");
			}
			printf("\n");
		}
		printf("\n");
		int cnt = 0;
		for (auto a : output) {
			if (a == 26) {//这里有神秘bug
				a = -2;
			}
			printf("%c", a);
			cnt++;
			//if (a < 0 || a>127) {
			//	cout << "?";
			//}
		}
		fclose(stdin);
		fclose(stdout);
		//freopen("CON", "r", stdin);
		//freopen("CON", "w", stdout);
	}

};

/*
解压
实例化Decode deco;
调用deco.work_decode(char * addr_source, char * addr_target)
其中addr_source为源文件地址、addr_target为目标文件地址
OrangeRain
*/
class Decode {
private:
	vector<char> table[130];//暴力建表
	vector<char> input;
	int sup0;

	unordered_map<string, char> mp;
public:
	void decode_work(char* adds, char* addt) {//试试吧
		freopen(adds, "r", stdin);
		freopen(addt, "w", stdout);
		mp.clear(); input.clear();
		char stmp[100];
		scanf("%d", &sup0); getchar();//读入补0
		for (int i = 0; i < 128; i++) {
			int x; string str = "";
			//cin >> x >> str;
			scanf("%d %s\n", &x, stmp);
			for (int i = 0; stmp[i] != '\0'; i++) {
				str += stmp[i];
			}
			if (str[0] != '-') {
				mp[str] = i;
			}

		}
		//getchar(); getchar();
		char c = getchar();
		while (c != EOF) {//不管怎么想都得专门为了EOF留一位，只能说寄。
			if (c == -2) c = 26;//神秘bug
			input.push_back(c);
			c = getchar();
		}

		vector<char> zhankai;//先把他全部展开成01的char吧
		vector<char> recover;
		for (int i = 0; i < input.size(); i++) {
			char a = input[i];
			if (i == input.size() - 1) {//把补的0判掉
				for (int i = 6; i > -1 + sup0; i--) {
					if ((1 << i) & a) {
						zhankai.push_back(1);
					}
					else {
						zhankai.push_back(0);
					}
				}
			}
			else {
				for (int i = 6; i > -1; i--) {
					if ((1 << i) & a) {
						zhankai.push_back(1);
					}
					else {
						zhankai.push_back(0);
					}
				}
			}
		}

		string str = "";
		for (auto a : zhankai) {
			str += a + '0';
			if (mp.count(str)) {
				char d = mp[str];
				if (d == 26) d = -2;//...
				printf("%c", d);
				str = "";
			}
		}
		fclose(stdin);
		fclose(stdout);

		freopen("CON", "r", stdin);
		freopen("CON", "w", stdout);
	}
};


//
//signed main() {
//	Encry enc;
//	char s1[10] = "1.txt";
//	char s2[10] = "jiami.txt";
//	char s3[10] = "yasuo.txt";
//	char s4[10] = "jieya.txt";
//	char s5[10] = "jiemi.txt";
//
//	Code co; Decode deco;
//
//	
//	enc.work_encry(s1, s2, 1234576, 0);
//	co.work_code(s2, s3);
//
//	deco.decode_work(s3, s4);
//	enc.work_encry(s4, s5, 1234576, 1);
//	return 0;
//}

