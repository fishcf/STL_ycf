#ifndef VECTOR_YCF_H
#define VECTOR_YCF_H
#include <iostream>
#include <cassert>
using namespace std;

template<class T> class vector_ycf  
{
public:
	enum {SAPRE_CAPACIY=5};
	typedef T* iterator;
	typedef const T* const_iterator;
    //构造函数
	explicit vector_ycf(int initsize = 0);
	vector_ycf(int initsize, T value);
	vector_ycf(iterator, iterator);
	vector_ycf(initializer_list<T> list);
	//复制控制
	vector_ycf(const vector_ycf &rhs);
	vector_ycf<T>& operator=(const vector_ycf &rhs);
	~vector_ycf() { delete[] elem; }
	//只读访接口
	bool empty()const { return thesize == 0; }
	int size()const { return thesize; }
	int disordered()const;
	int capacity()const { return thecapacity; }
	iterator begin() { return &elem[0]; }
	iterator end() { return &elem[thesize]; }
	const_iterator cbegin()const { return &elem[0]; }
	const_iterator cend()const { return &elem[thesize]; }
	int find(T const &value)const { return find(value, 0, thesize); }//无序向量整体查找
	int find(T const &value,int lo,int hi)const;
	int search(T const &value) const 
	{
		return (0 >= thesize) ? -1 : search(value, 0, thesize);
	}
	int search(T const &value, int lo, int hi)const;

	bool operator==(const vector_ycf<T> &rhs);
	bool operator!=(const vector_ycf<T> &rhs);

	void reserve(int newcapacity);
	void resize(int newsize,const T &newvalue);
	void resize(int newsize);
	void shrink();

	T& operator[](int index);
	const T& operator[](int index)const;
	T& front();
	const T& front() const;
	T& back();
	const T& back() const;

	void push_back(T value);
	iterator insert(iterator pos, const T &value);
	int insert(int r, const T &value);  //将value作为秩为r的元素插入
	void pop_back() { thesize--; }
	iterator erase(iterator b);
	T remove(int rank);
	int remove(int lo, int hi);
	void clear();
	void sort(int lo, int hi);
	void sort() { sort(0, thesize); }
	void unsort(int lo, int hi);
	int deduplicate();//无序去重
	int uniquify();//有序去重

	bool bubble(int lo, int hi);//扫描交换算法
	void bubble_sort(int lo, int hi);//气泡排序算法
	void merge(int lo,int mid,int hi);//归并算法
	void merge_sort(int lo, int hi);//归并排序算法
	int partition(int lo, int hi);//触点构造算法
	void quick_sort(int lo, int hi);//快速排序算法
	void heap_sort(int lo, int hi);//堆排序

private:
	T *elem;
	int thesize;
	int thecapacity;
};

/*------------------------------------------------构造函数------------------------------------------------*/
template<class T> vector_ycf<T>::vector_ycf(int initsize = 0) :thesize(initsize), thecapacity(initsize + SAPRE_CAPACIY)
{
	elem = new T[thecapacity];
	assert(!elem == NULL);
}
template<class T> vector_ycf<T>::vector_ycf(int initsize, T value) :thesize(initsize), thecapacity(initsize + SAPRE_CAPACIY)
{
	elem = new T[thecapacity];
	assert(!elem == NULL);
	for (int i = 0; i !=thesize; i++)
		elem[i] = value;
}
template<class T> vector_ycf<T>::vector_ycf(iterator beg, iterator end)
{
	thesize = end - beg;
	thecapacity = thesize + SAPRE_CAPACIY;
	elem = new T[thecapacity];
	assert(!elem == NULL);
	for (int i=0; beg!=end && i!=thesize; i++,beg++)
	{
		elem[i] = *beg;
	}
}
template<class T> vector_ycf<T>::vector_ycf(initializer_list<T> list)
{
	thesize = list.size;
	thecapacity = thesize + SAPRE_CAPACIY;
	elem = new T[thecapacity];
	assert(!elem == NULL);
	for (int i = 0, auto beg = list.begin(); i != thesize && beg != list.end(); i++, beg++)
		ele[i] = *beg;

}

template<class T> vector_ycf<T>::vector_ycf(const vector_ycf<T> &rhs)//复制构造函数
{
	thesize = rhs.thesize;
	thecapacity = rhs.thecapacity;
	elem = new T[thecapacity];
	assert(!elem == NULL);
	for (int i = 0; i != thesize; i++)
		elem[i] = rhs[i];
}
template<class T> vector_ycf<T>& vector_ycf<T>::operator=(const vector_ycf<T> &rhs)  //赋值操作符重载
{
	if (this != rhs)//防止自身赋值
	{
		delete[] elem;
		thesize = rhs.thesize;
	    thecapacity = rhs.thecapacity;
	    elem = new T[thecapacity];
	    assert(!elem == NULL);
		for (int i = 0; i != thesize; i++)
			elem[i] = rhs[i];
	}
	return *this;
}

/*-----------------------------------------------------关系操作符----------------------------------------------------*/
template<class T> bool vector_ycf<T>::operator==(const vector_ycf<T> &rhs)
{
	if (thesize == rhs.thesize)
	{
		int cnt = 0;
		for (int i = 0; i != thesize; i++)
			if (elem[i] == rhs[i])
				cnt++;
		if (cnt == thesize)
			return true;	
	}
	else return false;
}
template<class T> bool vector_ycf<T>::operator!=(const vector_ycf<T> &rhs)
{
	return !(*this == rhs);
}

template<class T> void vector_ycf<T>::reserve(int newcapacity)
{
	if (thesize <= newcapacity)
	{
		if (thesize < newcapacity / 2)
		{
			T *newarray = new T[newcapacity];
			int newsize = thesize > newcapacity ? newcapacity : thesize;
			for (int i = 0; i != newsize; i++)
				newarray[i] = elem[i];
			delete[] elem;
			elem = newarray;
			thecapacity = newcapacity;
		}
		return;
	}
	T *newarry = new T[newcapacity]; 
    for (int i = 0; i != thesize; i++)
		newarry[i] = elem[i];
	delete[] elem;
	elem = newarry;
	thecapacity = newcapacity;
}

template<class T> void vector_ycf<T>::resize(int newsize, const T &newvalue)
{
	if (newsize > thesize)
	{
		if (newsize>thecapacity)
			reserve(2 * newsize + 1);
		for (int i =0; i != newsize; i++)
			elem[i] = newvalue;
	}
	else if (newsize < thesize)
	{
		if (newsize < thesize / 2)
			reserve(newsize);
	}
	thesize = newsize;
}
template<class T> void vector_ycf<T>::resize(int newsize)
{
	resize(newsize, T());
}

template<class T> void vector_ycf<T>::shrink()
{
	if (thesize << 2 > thecapacity) return;
	int newcapacity = thecapacity >> 1;
	T* oldelem = elem;
	elem = new T[newcapacity];
	for (int i = 0; i != thesize; i++)
		elem[i] = oldelem[i];
	delete[] oldelem;
}






/*----------------------------------------------------访问操作------------------------------------------------------*/
template<class T> T& vector_ycf<T>::operator[](int index)
{
	if (index < 0 || index >= thesize)
	{
		throw out_of_range("超出范围");
	}
	return elem[index];
}
template<class T> const T& vector_ycf<T>::operator[](int index)const 
{
	if (index < 0 || index >= thesize)
	{
		throw out_of_range("超出范围");
	}
	return elem[index];
}
template<class T> T& vector_ycf<T>::front()
{
	if (!empty())
		return elem[0];
}
template<class T> const T& vector_ycf<T>::front() const
{
	if (!empty())
		return elem[0];
}
template<class T> T& vector_ycf<T>::back()
{
	if (!empty())
		return elem[thesize - 1];
}
template<class T> const T& vector_ycf<T>::back() const
{
	if (!empty())
		return elem[thesize - 1];
}
template<class T> int vector_ycf<T>::find(T const &value, int lo, int hi)const
{
	while ((value != elem[lo])&&(lo++ < hi));
	return lo < hi ? lo : -1;
}
template<typename T> int vector_ycf<T>::search(T const &value,int lo,int hi)const
{
	return (rand() % 2) ? binSearch(elem, lo, hi) : fibSearch(elem, lo, hi);
}
template<class T> int vector_ycf<T>::disordered()const
{
	int cnt = 0;
	for (int i = 1; i != thesize; i++)
		if (elem[i - 1] > elem[i])
			cnt++;
	return cnt;
}


/*-------------------------------------------------------插入操作----------------------------------------------------------*/
template<class T> void vector_ycf<T>::push_back(T value)
{
	if (thesize == thecapacity)
	{
		reserve(2 * thecapacity);
	}
	elem[thesize++] = value;
}
template<class T> T* vector_ycf<T>::insert(iterator pos, const T &value)
{
	if (pos<this->begin() || pos>this->end())
		cout << "超出范围";
	int num = b - elem;
	if (thesize == thecapacity)
	{
		reserve(2 * thecapacity);
		b = elem + num;
	}
	for (iterator p = this->end(); p > pos; p--)
		*p = *(p - 1);
	*pos = value;
	thesize++;
	return pos;
}
template<class T> int vector_ycf<T>::insert(int r, const T &value)  //将value作为秩为r的元素插入
{
	if (0<r || r>thesize)
		cout << "超出范围";
	if (thesize == thecapacity)         //若有必要，扩容
	{
		reserve(2 * thecapacity);
	}
	for (int i=thesize; i > r; i--)
		elem[i] = elem[i-1];
	elem[r]= value;
	thesize++;                         //置入新元素并更新容量
	return r;                          //返回秩
}
template<class T> T* vector_ycf<T>::erase(iterator pos)
{
	if (pos<this->begin() || pos>this->end())
		cout << "超出范围";
	iterator q = pos + 1;
	int num = q - elem;
	iterator p = this->end() - 1;
	for (; pos < p; pos++)
		*pos = *(pos + 1);
	thesize--;
	if (thesize < thecapacity / 4)
	{
		reserve(thesize + SPARE_CAPACITY);
		iterator q1 = elem + num;
		return q1;
	}
	return q;
}
template<class T> void vector_ycf<T>::clear()
{
	thesize = 0;
}
template<class T> int vector_ycf<T>::remove(int lo, int hi)//删除区间[lo,hi)
{
	if (lo == hi)
		return 0;
	while (hi<thesize)
		elem[lo++] = elem[hi++];
	thesize = lo;
	shrink();
	return hi - lo;
}
template<class T> T vector_ycf<T>::remove(int rank)
{
	T rem_val = elem[rank];
	remove(rank, rank + 1);
	return rem_val;
}
template<class T> int vector_ycf<T>::deduplicate()
{
	int oldsize = thesize, i = 1;
	while (i < thesize) 
		(find(elem[i], 0, i) < 0) ? i++ : remove(i);
	return oldsize - thesize;
}
template<typename T> int vector_ycf<T>::uniquify()
{
	/*   低效版
	int oldsize = thesize,i = 1;
	while (i < thesize)
		(elem[i - 1] == elem[i]) ? remove[i] : i++;
	return oldsize - thesize;
	*/
	int i=0,j=0;
	while (++j < thesize)
		if (elem[i] != elem[j])
			elem[++i] = elem[j];
	thesize = ++i;
	shrink();
	return j - i;
}
template<typename T> void vector_ycf<T>::sort(int lo,int hi)
{
	switch (rand() % 5)
	{
	case 1:bubble_sort(lo,hi); break;
	case 2:selection_sort(lo, hi);break;
	case 3:merge_sort(lo, hi); break;
	case 4:heap_sort(lo, hi); break;
	default:quick_sort(lo, hi); break;
	}
}
template<typename T> void vector_ycf<T>::bubble_sort(int lo, int hi)
{
	while (!(bubble(lo,hi--)));
}
template<typename T> bool vector_ycf<T>::bubble(int lo,int hi)
{
	bool sorted = true;
	while (++lo < hi)
	{
		if (elem[lo - 1] > elem[lo])
		{
			sorted = false;
			swap(elem[lo - 1], elem[lo]);
		}
	}
	return sorted;
}
template<typename T> void vector_ycf<T>::merge_sort(int lo, int hi)
{
	if ((hi - lo) < 2) return;
	int mid = (lo + hi) / 2;
	merge_sort(lo, mid);
	merge_sort(mid, hi);
	merge(lo, mid, hi);
}
template<typename T> void vector_ycf<T>::merge(int lo, int mid, int hi)
{
	T *A = elem + lo;
	int lb = mid - lo; T *B = new T[lb];
	for (int i = 0; i != lb; i++)
		B[i] = A[i];
	int lc = hi - mid; T *C = elem + mid;
	for (int i = 0, j = 0, k = 0; (j<lb)||(k<lc);)
	{
		if ((j<lb)&&(!(k<lc)||(B[j] <= C[k])))
			A[i++] = B[j++];
		if ((k<lc)&&(!(j<lb)||(C[k] < B[j])))
			A[i++] = C[k++];
	}
	delete[]B;
}

/*--------------------------------------------------------算法----------------------------------------------------------------*/
template<class T> void vector_ycf<T>::unsort(int lo,int hi)
{
	T* V = elem + lo;
	for (int i = hi - lo; i > 0; i--)
		swap(V[i - 1], V[rand() % i]);
}


/*---------------------------------------------------------非类模板成员函数-----------------------------------------------------------*/
template<typename T> void swap(T &v1, T &v2)
{
	T v3 = v1;
	v1 = v2;
	v2 = v3;
}

template<typename T> void permute(vector_ycf<T> &vec)//置乱算法
{
	for (int i = vec.size(); i > 0; i--)//自后向前
		swap(vec[i - 1], vec[rand() % i]);//vec[i-1]与vec[0,i)中某一随机元素交换
}
/*----------------二分查找（A版）
template<typename T> static int binSearch(vector_ycf<T> *A, T const &val, int lo, int hi)
{
	while (lo < hi)
	{
		int mid = (lo + hi) / 2;
		if (val < elem[mid])
			hi = mid;
		else if (elem[mid] < val)
			lo = mid+1;
		else return mid;
	}
	return -1;
}
*/
/* ----------------二分查找（版本B）
template<typename T> static int binSearch(vector_ycf<T> &A, T const val, int lo, int hi)
{
	while (1 < hi - lo)  //成功查找不能提前终止
	{
		int mid = (lo + hi)/2;
		(val < A[mid])? hi = mid : lo = mid;
	}
	return (val == A[lo]) ? lo : -1;
}
*/
template<typename T> static int binSearch(vector_ycf<T> &A, T const val, int lo, int hi)
{
	while (lo < hi)
	{
		int mid = (lo + hi) / 2;
		(val < A[mid]) ? hi = mid : lo = mid + 1;
	}
	return lo--;
}
#endif
