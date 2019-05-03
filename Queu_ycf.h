#include <iostream>
#include "list_ycf.h"


using namespace std;

template<class T> class Queue_ycf :public List_ycf<T>   //队列模板类（继承List_ycf原有接口)
{
public:  // size(),empty()等开放接口直接继承
	void enqueue(T const &e) { insertAsLast(e); } //入队：尾部插入
	T dequeue() { remove(first()); }              //出队：首部删除
	T& front() { return first()->data; }          //队首
};

/*-------------------------------------银行服务模拟-------------------------------------------------------------------*/
struct Customer  //顾客类
{
	int window;        //所属窗口（队列）
	unsigned int time; //服务时长
};
void simulate(int nWin, int servTime) //按 指定窗口数，服务总时间 模拟银行业务
{
	Queue_ycf<Customer>* windows = new Queue_ycf<Customer>[nWin]; //为每一窗口创建一个队列
	for (int now = 0; now < servTime; now++)   //在下班之前，每隔一个时间单位
	{
		if (rand() % (1 + nWin))  //新顾客以 nWin/(nWin+1)的概率到达
		{
			Customer c; c.time = 1 + rand() % 98; //新顾客到达，以服务时间随机确定
			c.window = bestWindow(windows, nWin); //找出最佳（最短）的服务窗口
			windows[c.window].enqueue(c);         //新顾客加入相应的队列
		}
		for (int i = 0; i < nWin; i++)   //分别检查各窗口队首客户是否结束服务
		{
			if (!windows[i].empty()) //各非空队列
			{
				if (--windows[i].front().time <= 0)  //队首顾客的服务时常减一
					windows[i].dequeue();            //服务完毕的客户出队，后继客户接替
			}
		}
	}
	delete[] windows;  //释放所有队列
}

int bestWindow(Queue_ycf<Customer> windows[], int nWin)    //为新顾客匹配最佳队列
{
	int minSize = windows[0].size(), optiWin = 0;//最优窗口
	for (int i = 0; i != nWin; i++)  //在所有窗口中
	{
		if (minSize > windows[i].size()) //挑选出
		{
			minSize = windows[i].size(); optiWin = i;  //队列最短者
		}
	}
	return optiWin; //返回
}
