// HEAD02.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include<string>
#include<iostream>
#include<fstream>
#include<time.h>
#include<map>
#include<set>
using namespace std;


int* p[5], f[5];//保存四个种群
int* c[2];
#define MAX_TABU_ITER 10000//tabu search最多的迭代次数
#define K 48  // 颜色数
#define ITER_CYCLE 10 //the number of tgenerations into one cycle
#define P1 0
#define P2 1
#define E1 2
#define E2 3
#define BEST 4
#define MAX_CROSS 3000

//#define SHOWBUG

//#define DEBUG
//#define SHOWF



int N;//顶点数
int* num_adj, **nb_v;//邻点数目与值
void init_graph(string fileName)
{
	ifstream ifs;
	ifs.open(fileName);
	string str;
	ifs >> str;
	while (!ifs.eof())
	{
		if (str == "edge")
		{
			ifs >> N;

			num_adj = new int[N];
			memset(num_adj, 0, N * sizeof(int));
			nb_v = new int*[N];
			for (int i = 0; i < N; i++)
			{
				nb_v[i] = new int[N - 1];
				memset(nb_v[i], 0, (N - 1) * sizeof(int));
			}
		}
		if (str == "e")
		{
			int m, n;  //顶点的ID
			ifs >> m >> n;
			m--;
			n--;
			//保存邻边信息
			nb_v[m][num_adj[m]] = n;
			num_adj[m]++;
			nb_v[n][num_adj[n]] = m;
			num_adj[n]++;
		}
		ifs >> str;
	}
	ifs.close();
#ifdef SHOWBUG
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < num_adj[i]; j++)
			cout << i + 1 << "\t" << nb_v[i][j] + 1 << endl;

	}
#endif // SHOWBUG
}

int** adj_table, **tt;
void Tabu(int* p, int& f);
void init_loc()
{
	adj_table = new int*[N];
	tt = new int*[N];
	for (int i = 0; i < N; i++)
	{
		adj_table[i] = new int[K];
		tt[i] = new int[K];
	}

	for (int i = 0; i < 5; i++)
	{
		p[i] = new int[N];
	}
	c[0] = new int[N];
	c[1] = new int[N];

}
void delete_loc()
{
	for (int i = 0; i < N; i++)
	{
		delete[]nb_v[i];
		delete[]adj_table[i];
		delete[]tt[i];
	}

	delete[]nb_v;
	delete[]adj_table;
	delete[]tt;
	delete[]num_adj;
	for (int i = 0; i < 4; i++)
	{
		delete[]p[i];
	}

	delete[]c[0];
	delete[]c[1];
}

int count_f(int* p)//计算冲突数
{
	int f = 0;
	int c_color, e_color, e_v;
	for (int i = 0; i < N; i++)
	{
		c_color = p[i];
		for (int j = 0; j < num_adj[i]; j++)
		{
			e_v = nb_v[i][j];
			e_color = p[e_v];
			if (c_color == e_color) f++;
		}
	}
	f = f / 2;
	return f;
}

void init_p(int m)
{
	for (int i = 0; i < N; i++)
		p[m][i] = rand() % K;

	f[m] = count_f(p[m]);
	//Tabu(p[m],f[m]);  //是否要在初始化时就禁忌搜索？？

#ifdef DEBUG
	for (int i = 0; i < N; i++)
		cout << "color of " << i + 1 << ": " << p[i] << endl;
#endif // DEBUG

}



void initilize(string filename)
{
	srand((unsigned int)time(NULL));
	init_graph(filename);
	init_loc();
	for (int i = 0; i < 5; i++)
	{
		init_p(i);
#ifdef SHOWF
		cout << f[i] << endl;
#endif // SHOWF

	}
}

//#define SHOWSET
set<int>::iterator it;
//交叉算子
void GPX(int* p1, int* p2, int *c)
{
	//map<int, int> p[2][K];
	set<int> p[2][K];
	int *s[2];
	int n1, n2;
	int color;
	s[0] = new int[N];
	s[1] = new int[N];
	memcpy(s[0], p1, N * sizeof(int));
	memcpy(s[1], p2, N * sizeof(int));

	for (int i = 0; i < N; i++)
	{
		color = p1[i];
		p[0][color].insert(i);
		color = p2[i];
		p[1][color].insert(i);
	}

#ifdef SHOWSET
	for (int j = 0; j < 2; j++)
	{
		for (int i = 0; i < K; i++)
		{
			it = p[j][i].begin();
			while (it != p[j][i].end())
			{
				cout << "颜色" << i << "顶点" << *it << endl;
				it++;
			}
		}
		cout << endl;
	}
	color = p1[1];
	//p[0][color].erase(1);//测试erase
#endif // SHOWSET
	int equ_k[2000];
	int max_size;
	int max_equ_count;
	int tmp;
	for (int l = 0; l < K; l++)
	{
		n1 = l % 2;
		n2 = (l + 1) % 2;
		max_size = -1;
		for (int i = 0; i < K; i++)
		{
			tmp = p[n1][i].size();
			if (tmp >= max_size)
			{
				if (tmp > max_size)
				{
					max_size = tmp;
					max_equ_count = 0;
				}
				equ_k[max_equ_count] = i;
				max_equ_count++;
			}
		}
		int del_color, del_v, del_color_2;
		del_color = equ_k[rand() % max_equ_count];//从最大颜色中随机选了一个颜色
		it = p[n1][del_color].begin();
		while (it != p[n1][del_color].end())
		{
			del_v = *it;
			del_color_2 = s[n2][del_v];
			p[n2][del_color_2].erase(del_v);
			c[del_v] = l;
			//cout << c[del_v] << endl;

			it++;
		}
		p[n1][del_color].clear();
	}


	for (int i = 0; i < K; i++)
	{
		it = p[0][i].begin();
		while (it != p[0][i].end())
		{
			c[*it] = rand() % K;
			it++;
		}
	}
	//for (int i = 0; i < N; i++)
	//	cout << c[i] << endl;

}

bool dH_equ_check(void)
{
	set<int> s[2][K];
	int color, size;
	for (int i = 0; i < N; i++)
	{
		color = p[0][i];
		s[0][color].insert(i);
		color = p[1][i];
		s[1][color].insert(i);
	}
	for (int i = 0; i < K; i++)
	{
		size = s[0][i].size();
		if (!size) continue;
		it = s[0][i].begin();
		for (int j = 0; j < K; j++)
		{
			if (s[1][j].count(*it))//*it出现过
			{
				if (size == s[1][j].size())
				{
					it++;
					//iter2 = p[1][j].begin();
					while (it != s[0][i].end())
					{
						if (!s[1][j].count(*it)) //p1中元素在p2中未出现
							return false;
						it++;
					}
					s[1][j].clear();//不能真清除原有的了
					break;
				}
				else
					return false;
			}
		}
	}
	return true;
}


//禁忌搜索
void init_adj(int* p)
{
	for (int i = 0; i < N; i++)
		memset(adj_table[i], 0, K * sizeof(int));

	int c_color;
	int e_v, e_color;//邻点，邻点颜色

	for (int i = 0; i < N; i++)
	{
		c_color = p[i];
		for (int j = 0; j < num_adj[i]; j++)
		{
			e_v = nb_v[i][j];
			e_color = p[e_v];
			adj_table[i][e_color]++;
		}
	}
}

void init_tt()
{
	for (int i = 0; i < N; i++)
		memset(tt[i], 0, K * sizeof(int));
}


int best_f;
int delt;
int iter;
int equ_count;
int equ_delt[2000][2];//先试试写死
int sel_vertex, sel_color;
void findmove(int *p, int &f)
{
	delt = 10000;//初始为最大整数
	int tmp_delt;
	int c_color;//当前结点颜色
	int *h_color;//邻接颜色表行首指针
	int *h_tabu;//禁忌表行首指针
	int c_color_table;//当前结点邻接颜色表的值
	for (int i = 0; i < N; i++) {
		c_color = p[i];
		h_color = adj_table[i];
		c_color_table = h_color[c_color];//即adj_color_table[i][sol[i]]的值
		if (c_color_table > 0) {  //颜色表此处的值不为0
			h_tabu = tt[i];
			for (int j = 0; j < K; j++) {
				if (c_color != j) { //如果颜色不相同
					tmp_delt = h_color[j] - c_color_table;
					if (tmp_delt <= delt && (iter > h_tabu[j] || (tmp_delt + f) < best_f)) {
						if (tmp_delt < delt) {//当前解小于本次迭代最优解,则重新开始保存解

							equ_count = 0;
							delt = tmp_delt;

						}
						equ_delt[equ_count][0] = i;
						equ_delt[equ_count][1] = j;
						equ_count++;//end of another way
					}
				}
			}
		}
	}

	int tmp = rand() % equ_count;//有多个解时，随机选择
	sel_vertex = equ_delt[tmp][0];
	sel_color = equ_delt[tmp][1];
}

void makemove(int* p, int &f)
{
	f = delt + f;
	if (f < best_f) best_f = f;
	int old_color = p[sel_vertex];
	p[sel_vertex] = sel_color; //更新颜色
	tt[sel_vertex][old_color] = iter + 0.6* f + (rand() % 10);//更新禁忌表
																	   //还要有数组存下每个顶点的邻边，以及邻边的数量
	int* h_NbID = nb_v[sel_vertex];
	int num_edge = num_adj[sel_vertex];
	int tmp;
	for (int i = 0; i < num_edge; i++)
	{//更新邻接颜色表
		tmp = h_NbID[i];//等于nbID[sel_vertex][i]
		adj_table[tmp][old_color]--;
		adj_table[tmp][sel_color]++;

	}
}

void Tabu(int* p, int& f)
{
	best_f = f;
	init_adj(p);
	init_tt();
	iter = 0;
	while (iter < MAX_TABU_ITER && best_f)
	{
		findmove(p, f);
		makemove(p, f);
		iter++;
	}
#ifdef SHOWF
	cout << "f:" << f << endl;
	cout << endl;
#endif

}


void copy_popu(int dst, int src)
{
	memcpy(p[dst], p[src], N * sizeof(int));
	f[dst] = f[src];
}


//#define TEST_DH

int main()
{
	int count = 0;
	clock_t start, end;
	string filename = "DSJC500.5.col";
	std::cout << "file:" << filename << "\t k=" << K << "\t L=" << MAX_TABU_ITER << endl;
	while (count != 20)
	{
		initilize(filename);
		int generation = 0, cycle = 0;
		start = clock();
		do
		{
			GPX(p[P1], p[P2], c[0]);
			GPX(p[P2], p[P1], c[1]);
			int f_tmp;

			f_tmp = count_f(c[0]);
			Tabu(c[0], f_tmp);
			memcpy(p[0], c[0], N * sizeof(int));
			f[0] = f_tmp;

			f_tmp = count_f(c[1]);
			Tabu(c[1], f_tmp);
			memcpy(p[1], c[1], N * sizeof(int));
			f[1] = f_tmp;
			////////////////////////////

			int best_p = 0, min_f = 10000;
			for (int i = 0; i < 3; i++)
			{
				if (min_f > f[i])
				{
					min_f = f[i];
					best_f = i;
				}
			}
			copy_popu(E1, best_f);

			if (f[BEST] > f[E1]) copy_popu(BEST, E1);

			if (generation%ITER_CYCLE == 0)
			{
				copy_popu(P1, E2);
				copy_popu(E2, E1);
				init_p(E1);
				cycle++;
			}
			generation++;
		} while (f[BEST] && !dH_equ_check() && generation < MAX_CROSS);

		end = clock();
		std::cout << "NO." << count << "\t generation= " << generation << "\t time=" << (end - start) << endl;
		count++;
	}







#ifdef TEST_DH

	N = 5;
	for (int i = 0; i < 4; i++)
	{
		p[i] = new int[N];
	}

	p[0][0] = 0;
	p[0][1] = 0;
	p[0][2] = 0;
	p[0][3] = 2;
	p[0][4] = 3;

	p[1][0] = 3;
	p[1][1] = 3;
	p[1][2] = 3;
	p[1][3] = 1;
	p[1][4] = 2;

	bool equal;
	equal = dH_equ_check();
	cout << endl;
#endif



	delete_loc();

	return 0;
}

