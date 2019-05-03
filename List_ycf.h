include <iostream>
#include <string>

using namespace std;
typedef int Rank;

template<class T> class ListNode
{
public:
	ListNode() {}//默认构造函数
	ListNode(T e, ListNode<T>* p = NULL, ListNode<T>* s = NULL) :data(e), prev(p), succ(s) {}

	ListNode<T>* insertAsPrev(T const &e);//当前节点前插入新节点
	ListNode<T>* insertAsSucc(T const &e);//当前结点后插入新节点

	T data;
	ListNode<T>* prev;
	ListNode<T>* succ;
};

template<class T> class List_ycf
{
private:
	int _size;//规模
	ListNode<T> *header;//头节点
	ListNode<T> *trailer;//尾节点

protected:
	void init();//初始化列表
	int clear();//列表清空
	void copyNodes(ListNode<T> *p, int n);//复制位置p开始的n项
	void merge(ListNode<T> *p, int n, List_ycf<T> &L, ListNode<T> *q, int m);//有序区间归并
	void mergesort(ListNode<T> *, int);//对p开始的n个节点归并排序
	void selectionsort(ListNode<T> *, int);
	void insertionsort(ListNode<T> *, int);

public:
	//构造函数
	List_ycf() { init(); }//默认构造函数
	List_ycf(List_ycf<T> const &L);//整体复制
	List_ycf(List_ycf<T> const &L, Rank r, int n);//复制L从第r项开始的n项            可能有问题
	List_ycf(ListNode<T>* p, int n);//复制从位置p开始的n项

	//析构函数
	~List_ycf();

	//只读访问接口
	Rank size() { return _size; }//规模
	bool empty() { return _size == 0; }//判空
	T& operator[](Rank r)const;//[]操作符重载
	ListNode<T>* first() const { return header->succ; }//首节点
	ListNode<T>* last() const { return trailer->prev; }//末节点
	bool valid(ListNode<T> *p)//判断p对外是否合法
	{
		return p && (p != header) && (p != trailer);
	}
	int disorder()const;//判断列表是否已排序
	ListNode<T>* find(T const &e, int n, ListNode<T>* p)const;//无序区间查找(在p的n个前驱中找到等于e的最后者)
	ListNode<T>* find(T const &e) //无序列表查找
	{
		return find(T const &e, _size, trailer);
	}
	ListNode<T>* search(T const &e, int n, ListNode<T>*);//有序区间查找
	ListNode<T>* search(T const &e)                      //有序列表查找
	{
		search(T const &e,_size,trailer);
	}
	ListNode<T>* selectMax(ListNode<T>* p, int n);//在位置p及其n-1个后继中寻找最大值
	ListNode<T>* selectMax()                      //整体最大值
	{
		searchMax(first(), _size);
	}

	//可写访问接口
	ListNode<T>* insertAsFirst(T const &e);//将e作为首节点插入
	ListNode<T>* insertAsLast(T const &e);//将e作为尾节点插入
	ListNode<T>* insertBefore(ListNode<T>* p, T const &e);//将e作为p的前驱插入
	ListNode<T>* insertAfter(ListNode<T>* p, T const &e);//将e作为p的后继插入
	T remove(ListNode<T>* p);//删除合法位置p处的节点，返回被删除节点
	void merge(List_ycf<T> &L) { merge(first(), _size, L, L.first(), L._size); }//全列表归并
	void sort(ListNode<T>* p, int n);//区间排序
	void sort() { sort(first(), _size); }//整体排序
	int deduplicate();//无序去重
	int uniquify();//有序去重
	void reverse();//倒置

	//遍历
	void traverse(void(*)(T&));//遍历,依次实施visit操作（函数指针，只读或局部性修改）
	template<typename VST> void traverse(VST&);//遍历器遍历，依次实施visit操作，（函数对象，可全局性修改）

};

/*-----------------------------------------------查找-----------------------------------------------------------*/
template<typename T> //在无序列表内节点p的n个前驱中查找等于e的最后者
ListNode<T>* List_ycf<T>::find(T const &e, int n, ListNode<T>* p)const 
{
	while (0 < n--)//对于p的n个前驱
		if (e == (p = p->prev)->date)//逐一比对
			return p;//成功，返回命中位置
	return NULL;//失败
}
template<typename T>  //在有序列表内节点p的n个前驱中查找不大于e的最后者
ListNode<T>* List_ycf<T>::search(T const &e, int n, ListNode<T>* p)
{
	while (0 < n--)//对于p的n个前驱，从右向左逐个比较
		if ((p = p->prev)->data <= e)//直至命中，数值越界或范围越界
			break;
	return p;//返回查找终止的位置
}

/*------------------------------------------------------插入-----------------------------------------------------------*/
template<typename T> ListNode<T>* List_ycf<T>::insertAsFirst(T const &e)//将e作为首节点插入
{
	_size++;
	return header->insertAsSucc(e);
}
template<typename T> ListNode<T>*  List_ycf<T>::insertAsLast(T const &e)//将e作为末节点插入
{
	_size++;
	return trailer->insertAsPrev(e);
}
template<typename T> ListNode<T>* List_ycf<T>::insertBefore(ListNode<T>* p, T const &e)//e当作p的前驱插入
{
	_size++;
	return p->insertAsPrev(e);
}
template<typename T> ListNode<T>* List_ycf<T>::insertAfter(ListNode<T>* p, T const &e)//e当作p的后继插入
{
	_size++;
	return p->insertAsSucc(e);
}
template<typename T> ListNode<T>* ListNode<T>::insertAsPrev(T const &e)  //前插入
{
	ListNode<T>* x = new ListNode(e, prev, this);//创建新节点
	prev->succ = x;prev = x;//设置顺向连接
	return x;//返回新节点位置
}
template<typename T> ListNode<T>* ListNode<T>::insertAsSucc(T const &e) //后插入
{
	ListNode<T>* x = new ListNode(e, this, succ);//创建新节点
	succ->prev = x; succ = x;//设置逆向连接
	return x;//返回新节点位置
}

/*-----------------------------------------复制控制--------------------------------------------------------*/
template<typename T> void List_ycf<T>::init()//列表初始化
{
	header = new ListNode<T>;//头节点
	trailer = new ListNode<T>;//尾节点
	header->prev = NULL; header->succ = trailer;
	trailer->prev =header; trailer->succ = NULL;
	_size = 0;//规模为0
}
template<typename T> T& List_ycf<T>::operator[](Rank r)const        //操作符重载
{
	ListNode<T> *p = first(); //p指向首节点
	while (0 < r--)
		p = p->succ;
	return p->data;          //返回秩为r的节点的数据
}
template<typename T> void List_ycf<T>::copyNodes(ListNode<T>* p, int n) //复制列表中自p起的n项
{
	init();//初始化列表
	while (n--)
	{   //将自p起的n项作为末节点插入
		insertAsLast(p->data);
		p = p->succ;
	}
}
template<typename T> List_ycf<T>::List_ycf(ListNode<T>* p, int n)  //复制自p起的n项
{
	copyNodes(p, n);
}
template<typename T> List_ycf<T>::List_ycf(List_ycf<T> const &L)      //整体复制列表
{
	copyNodes(L.first(), L._size);
}
template<typename T> List_ycf<T>::List_ycf(List_ycf<T> const &L, Rank r, int n)  //复制L中自第r项开始的n项
{
	ListNode<T>* p = L.first();
	while (0<r--)
		p = p->succ;
	copyNodes(p, n);
}
template<typename T> List_ycf<T>::~List_ycf()   //析构函数
{
	clear(); delete header; delete trailer;
}

/*-----------------------------------------------删除---------------------------------------------------------------*/
template<typename T> T List_ycf<T>::remove(ListNode<T>* p)
{
	T e = p->data;//备份带删除节点的数值，以便返回
	p->prev->succ = p->succ;
	p->succ->prev = p->prev;
	delete p; _size--;//释放节点，更新规模
	return e;//返回被删除节点的数值
}
template<typename T> int List_ycf<T>::clear()  //清空列表
{
	int OldSize = _size;
	while (0<_size)          //反复删除首节点，直至列表变空
		remove(header->succ);
	return OldSize;
}

/*-----------------------------------------------------唯一化----------------------------------------------------*/
template<typename T> int List_ycf<T>::deduplicate()//剔除无序列表中的重复节点，返回被删除元素的总数
{
	if (_size < 2) return 0;//自然有序
	int OldSize = _size;
	ListNode<T>* p = header; Rank r = 0;//从首节点开始
	while (trailer != (p=p->succ))//一直到末节点
	{
		ListNode<T>* q = find(p->data, r, p);//在p的r个前驱中查找p的数值
		q ? remove(q) : r++;//若q存在，则删除；若p不存在，r++
	}
	return Oldsize - _size;//返回被删除元素的总数
}
template<typename T> int List_ycf<T>::uniquify() //剔除有序列表中的重复节点，返回被删除元素的总数
{
	if (_size < 2) return 0;//自然有序
	int OldSize = _size;
	ListNode<T>* p; ListNode<T>* q;//依次指向相邻的节点
	for(p=header,q=p->succ;q!=trailer;p=q,q=q->succ)//自左向右扫描
		if (p->data == q->date)
		{
			remove(q->data);    //若两者雷同，则删除后者
			q = p;
		}
	return OldSize - _size;
}

/*------------------------------------------------------遍历----------------------------------------------------------*/
template<typename T> void List_ycf<T>::traverse(void(*visit)(T&))  //利用函数指针机制的遍历
{
	for (ListNode<T>* p = header; p != trailer; p = p->succ)
		visit(p->data);
}
template<typename T> template<typename VST> void List_ycf<T>::traverse(VST &visit)   //利用函数对象机制的遍历
{
	for (ListNode<T>* p = header; p != trailer; p = p->succ)
		visit(p->data);
}

/*------------------------------------------------------排序器---------------------------------------------------------*/
template<typename T> void List_ycf<T>::sort(ListNode<T>* p,int n)   //列表区间排序
{
	switch (rand() % 3)  //随机选组排序算法
	{
	case 1: insertionsort(p, n); break;   //插入排序
	case 2: selectionsort(p, n); break;   //选择排序
	default:mergesort(p, n); break;       //归并排序
	}
}
template<typename T>  //列表插入排序：对起始于位置p的n个元素排序
void List_ycf<T>::insertionsort(ListNode<T>* p, int n)
{
	for (int r = 0; r != n; r++)//逐一为各个节点
	{
		insertAfter(search(p->data, r, p), p->data);//查找适当的位置并插入
		p = p->succ;                                //转向下一节点
		remove(p->prev);
	}
}
template<typename T>  //列表选择排序：对起始于位置p的n个元素排序
void List_ycf<T>::selectionsort(ListNode<T>* p, int n)
{
	ListNode<T>* head = p->prev;; List_ycf<T>* tail = p;
	for (int i = 0; i != n; i++)
		tail = tail->succ;       //待排序区间为（head，tail）
	while (1 < n)
	{
		ListNode<T>* max = selectMax(head->succ, n);//找出最大者（歧义者后者优先）
		insertBefore(tail, remove(max));//将其移至区间末尾（作为有序区间的首元素）
		tail = tail->prev;n--;
	}
}
template<typename T> ListNode<T>* List_ycf<T>::selectMax(ListNode<T>* p, int n)   //从起始于位置p的n个元素中找出最大者
{
	ListNode<T>* max = p;  //最大者暂定为首节点
	for (ListNode<T>* cur = p; 1 < n; n--)//从p开始逐一与max比较
	{
		if ((cur->succ->data) >= (max->data)) //若当前元素不小于max
			max = cur;                        //则更新最大元素位置记录
	}
	return max;//返回最大节点位置
}
template<typename T> //有序列表的归并：当前列表中自p起的n个元素，与列表L中自q起的m个元素归并
void List_ycf<T>::merge(ListNode<T>* p, int n,List_ycf<T>& L,ListNode<T>* q,int m)
{
	ListNode<T>* pp = p->prev;  //借助前驱，以便返回前。。。
	while (0 < m)   //在q尚未移除区间之前
	{
		if ((0 < n) && (p->data <= q->data)) //若p仍在区间内且 v（p）<= v(q)，则
		{
			if (q == (p = p->succ))         //将p替换为其直接后继，若p==q，说明已排序好，直接退出
				break;
			n--;
		}
		else    //若p超出区间或 v(p) > v(q),则
		{
			insertBefore(p, L.remove((q = q->succ)->prev)); //将q插入到p之前
			m--;
		}
		p = pp->succ;//确定归并后区间的新起点
	}
}
template<typename T> void List_ycf<T>::mergesort(ListNode<T>* p, int n)
{
	if (n < 2) return;//自然有序
	int m = n >> 1;  //以中点为界
	ListNode<T>* q = p;
	for (int i = 0; i < m; i++)//均分列表
		q = q->succ;
	mergesort(p, m);  
	mergesort(q, n - m);  //对前后子序列分别进行排序
	merge(p, n, *this, q, n - m);           //归并
}
