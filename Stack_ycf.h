#include <iostream>
#include "vector_ycf.h"
#include <cstring>

using namespace std;

template<class T> class Stack_ycf :public vector_ycf<T> //以向量为基类，派生出栈模板类
{
public:   //size(),empty()等开放接口直接继承
	void push(T const &e) { insert(size(), e); }  //入栈：等效于将新元素作为向量的末元素插入
	T pop() { return remove(size() - 1); } //出栈：等效于删除向量的末元素
	T& top() { return (*this)[size() - 1]; }  //取栈顶元素：直接返回向量的末元素

};



/*------------------------------------------------------进制转换------------------------------------------------------------*/
/*
void convert(Stack_ycf<char> &S, int64_t n, int base)  //十进制数n到 base进制数 的转换（递归版）
{   // 0<n , 1<base<=16,新进制下的数位符号，可视base的取值范围适当扩充
	static char digit[] = { '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F' };
	if (0 < n)   //在尚有余数的情况下
	{
		convert(S, n / base, base); //通过递归得到所有更高位
		S.push(digit[n%base]);      //输出低位
	}
}//新进制下由高到低的各数位，自顶而下保存于栈中
*/

void convert(Stack_ycf<char> &S, int64_t n, int base)  //十进制数n到 base进制数 的转换（迭代版）
{   // 0<n , 1<base<=16,新进制下的数位符号，可视base的取值范围适当扩充
	static char digit[] = { '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F' };
	while (0 < n)   //由低至高，逐一计算出新进制下的各数位
	{
		int remainder = (int)(n%base);S.push(digit[remainder]);//余数（当前位）入栈
		n /= base;  //n更新为对base的除商
	}
}//新进制下由高到低的各数位，自顶而下保存于栈中


/*---------------------------------------------------括号匹配--------------------------------------------------------------*/
void trim(const char exp[], int &lo, int &hi)  //删除表达式中不包含括号的最长前缀，最长后缀
{
	while ((lo <= hi) && (exp[lo] != '(') && exp[lo] != ')') lo++;//查找第一个
	while ((lo <= hi) && (exp[hi] != '(') && exp[hi] != ')') hi--;//和最后一个括号
}
int divide(const char exp[], int lo, int hi)  //切分表达式exp[lo,hi],是exp匹配仅当他们匹配
{
	int mi = lo; int crc = 1;//crc为[lo,mi]范围内左、右括号数目之差
	while ((0 < crc) && (++mi < hi))  //逐个检查各字符，知道左、右括号数目相等，或着越界
	{
		if (exp[mi] == '(') crc++;   //左右括号分别计数
		if (exp[mi] == ')') crc--;
	}
	return mi; //若mi<=hi,则为合法切分点；否则，意味着局部可能不匹配
}

/*
bool paren(const char exp[], int lo, int hi)      //检查表达式exp[lo,hi]是否匹配（递归版）
{
	trim(exp, lo, hi);if (lo > hi) return true;  //清除不包含括号的前、后缀
	if (exp[lo] != '(') return false;            //首字符非左括号，则必不匹配
	if (exp[hi] != ')') return false;            //末字符非右括号，则必不匹配
	int mi = divide(exp, lo, hi);
	if (mi > hi) return false;                   //切分点不合法，意味着局部以致整体不匹配
	return paren(exp, lo + 1, mi - 1) && paren(exp, mi + 1, hi - 1); //分别检查 左、右子表达式
}
*/
bool paren(const char exp[], int lo, int hi)       //检查表达式exp[lo,hi]是否匹配，可兼顾3种括号
{
	Stack_ycf<char> S;  //使用栈记录当前尚未匹配的左括号
	for(int i=0;exp[i];i++)  //逐一检查当前字符
		switch (exp[i])      //左括号直接进栈，后括号若与栈顶失配，则表达式必不匹配
		{
		case '(':case '[':case '{': S.push(exp[i]); break;
		case ')': if (('(' != S.pop()) || S.empty()) return false; break;
		case '}': if (('{' != S.pop()) || S.empty()) return false; break;
		case ']': if (('[' != S.pop()) || S.empty()) return false; break;
		default: break;      //非括号字符一律忽略
		}
	return S.empty();  //整个表达式扫描过后，栈中若仍残留（左）括号，则不匹配；若栈空，则匹配；
}

/*--------------------------------------------表达式求值------------------------------------------------------------------*/
#define N_OPTR 9    //运算符总数
typedef enum {ADD,SUB,MUL,DIV,POW,FAC,L_P,R_P,EOE}Operator;  //运算符集合  FAC:阶乘   L_P :左括号 R_P :右括号  EOE :终止符
const char pri[N_OPTR][N_OPTR] = //运算符优先等级 [栈顶] [当前]
{ 
	/*         |-------------- 当前运算符 --------------| */
	/*            +    -    *    /    ^    !    (    )   \0   */
	/* -- + */   '>', '>', '<', '<', '<', '<', '<', '>', '>',
	/* |  - */   '>', '>', '<', '<', '<', '<', '<', '>', '>',
	/* 栈 * */   '>', '>', '>', '>', '<', '<', '<', '>', '>',
	/* 顶 / */   '>', '>', '>', '>', '<', '<', '<', '>', '>',
	/* 运 ^ */   '>', '>', '>', '>', '>', '<', '<', '>', '>',
	/* 算 ! */   '>', '>', '>', '>', '>', '>', ' ', '>', '>',
	/* 符 ( */   '<', '<', '<', '<', '<', '<', '<', '=', ' ',
	/* |  ) */   ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
	/* - \0 */  '<', '<', '<', '<', '<', '<', '<', ' ', '='
	};

void readNumber(char *p, Stack_ycf<float>& stk)  //将起始于p的子串分解为数值，并存入操作数栈
{
	stk.push((float)(*p - '0')); //当前数位对应的数值进栈
	while (isdigit(*(++p))) //只要后续还有紧邻的数字（即多位整数的情况），则
		stk.push(stk.pop() * 10 + (*p - '0')); //弹出原操作数并追加新数位后，新数值重新如栈
	if ('.' != *p) //若后续非小数点，则意味着当前操作数解析完毕
		return;   
	else  //否则，意味着还有小数部分
	{
		float fraction = 1;
		while (isdigit(*p++))  //诸位加入
			stk.push(stk.pop() + (*p - '0') *(fraction/=10));  //小数部分
	}
}

int optr2rank(char op)
{
	switch (op)
	{
	case '+': return ADD;
	case '-': return SUB;
	case '*': return MUL;
	case '/': return DIV;
	case '^': return POW;
	case '!': return FAC;
	case '(': return L_P;
	case ')': return R_P;
	case '\0': return EOE;
	default: exit(-1);    //未知运算符
	}
}
char orderBetween(char op1, char op2)  //比较两个运算符之间的优先级
{
	return pri[optr2rank(op1)][optr2rank(op2)];
}

void append(char* & rpn, float opnd) //将操作数接至RPN末尾
{
	int n = strlen(rpn);  //rpn当前长度（以'\0'结尾，长度n+1)
	char buf[64];
	if (opnd != (float)(int)opnd)
		sprintf_s(buf, ".2f\0", opnd);   //浮点格式
	else
		sprintf_s(buf, "%d\0", opnd);    //整数格式
	rpn = (char*)realloc(rpn, sizeof(char)*(n + strlen(buf)+1)); //扩展空间
	strcat_s(rpn,strlen(rpn), buf); //RPN加长
}
void append(char* &rpn, char optr)  //将操作符接至RPN末尾
{
	int n = strlen(rpn); //RPN当前长度（ 以'\0'结尾，长度为n+1)
	rpn = (char*)realloc(rpn, sizeof(char)*(n + 3)); //n+1+1(optr)+1('\0')
	sprintf_s(rpn + n, sizeof(char)*(n + 3), "%c", optr); //扩展空间
	rpn[n + 2] = '\0'; //接入指定的运算符
}
float fac(float num)   //计算阶乘
{
	if (num == 0)
		return 1;
	return num*fac(num - 1);
}

float caclu(const char optr, float opnd)  //一员操作符(即阶乘)运算
{
	if (optr == '!')
		return fac(opnd);
	else
		return -1;
}
float caclu(float opnd1, const char optr, float opnd2)
{
	switch (optr)
	{
	case '+': return opnd1 + opnd2;
	case '-': return opnd1 - opnd2;
	case '*': return opnd1*opnd2;
	case '/': return opnd1 / opnd2;
	case '^': return pow(opnd1, opnd2);
	}
	return -1;
}

float evaluate(char* S, char*& RPN)  //对（已剔除白空格的）表达式S求值，并转换为 逆波兰式RPN
{
	Stack_ycf<float> opnd; Stack_ycf<float> optr; //运算数栈、运算符栈
	optr.push('\0');  //尾哨兵‘0\'也作为头哨兵入栈
	while (!optr.empty())     //在运算符栈非空之前，逐个处理表达式中各字符
	{
		if (isdigit(*S))      //若当前字符为操作数，则
		{ 
			readNumber(S, opnd); append(RPN, opnd.top());//读入操作数，并将其接至RPN末尾
		}  	
		else  //若当前字符为运算符，则
			switch (orderBetween(optr.top(), *S)) // 视当前运算符与栈顶运算符优先级高低分别处理
			{
			case '<': //栈顶运算符优先级更低时
				optr.push(*S); S++; //计算推迟，当前运算符进栈
				break;
			case '=': //优先级相等（当前运算符为右括号或者尾部哨兵‘\0'时）
				optr.pop(); S++;  //脱括号并接受下一字符
				break;
			case '>':  //栈顶优先级更高时，可实施相应的计算，并将结果重新入栈
			{
				char op = optr.pop(); append(RPN, op); //栈顶运算符出栈并续接至RPN末尾
				if ('!' == op)   //若属于一元操作符
				{
					float pOpnd = opnd.pop();      //只需取出一个操作数，并
					opnd.push(caclu(op, pOpnd));   //实施一元计算，结果入栈
				}
				else
				{
					float pOpnd2 = opnd.pop(); float pOpnd1 = opnd.pop();  //取出后，前操作数
					opnd.push(caclu(pOpnd1, op, pOpnd2));            //进行二元计算，结果入栈
				}
				break;
			}
			default: exit(-1);  //缝语法错误，不做处理直接退出
			}
	}
	return opnd.pop(); //弹出并返回最后的计算结果
}

/*----------------------------------------------------八皇后-------------------------------------------------*/
struct Queen //皇后类
{
	int x, y;//皇后在棋盘上的坐标
	Queen(int xx=0, int yy=0) :x(xx), y(yy) {};
	bool operator==(Queen const& q)const
	{
		return (x == q.x)   //行冲突
			|| (y == q.y)   //列冲突
			|| (x + y == q.x + q.y)   //沿正对角线冲突
			|| (x - y == q.x - q.y);  //沿反对角线冲突
	}
	bool operator!=(Queen const &q)const { return !(*this == q);}   //重载不等操作符
};
void placeQueens(int N)   //N皇后算法（迭代版）：采用试探回溯的策略，借助栈记录查找的结果
{
	Stack_ycf<Queen> solu;  //存放（部分）结果的栈
	Queen q(0, 0);          //从原点出发
	do   //反复试探，回溯
	{
		if (N < solu.size() || N <= q.y) //若已出界，则
		{
			q = solu.pop(); q.y++;   //回溯一行，并继续试探下一列
		}
		else
		{
			while ((q.y < N) && (0 <= solu.find(q)))        //find()函数中用到了重载的 != 操作符做判断
			{
				q.y++; /*nCheck++*/;  //尝试找到可摆放下一皇后的列
			}
			if (N > q.y)
			{
				solu.push(q);
				//if (N <= solu.size()) //若部分解已成为全局解，则通过全局变量nSolu计数
				//	nSolu++;
				q.x++; q.y = 0;  //转入下一行，从第0列开始，试探下一皇后
			}
		}
	} while ((0 < q.x) || (q.y < N));  //所有分支均已 或穷举 或剪枝 之后，算法结束
};

/*-----------------------------------迷宫寻径----------------------------------------------------------*/
typedef enum{AVAILABLE,ROUTE,BACKTRACKED,WALL} Status;   //迷宫单元状态
// 原始可用的，当前路径上的，所有方向均尝试失败后回溯的，不可使用的（墙）

typedef enum{UNKNOW,EAST,SOUTH,WEST,NORTH,NO_WAY} ESWN;   //单元的相对邻接方向
//             未定，东，南，西，北，无路可通

inline ESWN nextESWN(ESWN eswn) { return ESWN(eswn + 1); }

struct Cell      //迷宫格点
{
	int x, y; Status status;  //x坐标，y坐标，类型
	ESWN incoming, outgoing;  //进入，走出方向
};
#define LABY_MAX 24
Cell laby[LABY_MAX][LABY_MAX]; //迷宫

inline Cell* neighbor(Cell* cell)    //查询当前位置的相邻节点
{
	switch (cell->outgoing)
	{
	case EAST: return cell + LABY_MAX;   //向东
	case SOUTH: return cell + 1;         //向南
	case WEST: return cell - LABY_MAX;   //向西
	case NORTH: return cell - 1;         //向北
	default: exit(-1);
	}
}

inline Cell* advance(Cell *cell)
{
	Cell *next;
	switch (cell->outgoing)
	{
	case EAST:  next = cell + LABY_MAX; next->incoming = WEST;  break; //向东
	case SOUTH: next = cell + 1;        next->incoming = NORTH; break; //向南
	case WEST:  next = cell - LABY_MAX; next->incoming = EAST;  break; //向西
	case NORTH: next = cell - 1;        next->incoming = SOUTH; break; //向北
	default: exit(-1);
	}
	return next;
}

bool labyrith(Cell Laby[LABY_MAX][LABY_MAX], Cell *s, Cell *t)  //迷宫寻径算法：在格单元s到t之间规划一条道路
{
	if ((AVAILABLE != s->status) || (AVAILABLE != t->status)) 
		return false;            //退化情况
	Stack_ycf<Cell*> path;       //用栈记录通路
	s->incoming = UNKNOW; s->status = ROUTE; path.push(s); //起点
	do
	{
		Cell* c = path.top();    //检查当前位置
		if (c == t) return true; //若已到达终点，则找到了一条通路；否则，（沿未试探的方向）向前试探
		while (NO_WAY > (c->outgoing = nextESWN(c->outgoing)))//注意检查所有方向
			if (AVAILABLE == neighbor(c)->status) break;     //试图找到尚未试探的方向
		if (NO_WAY <= c->outgoing) //若所有方向都试探过了
		{
			c->status = BACKTRACKED; c = path.pop();  //则向后回溯一步
		}
		else           //否则，向前试探一布
		{
			path.push(c = advance(c)); c->outgoing = UNKNOW; c->status = ROUTE;  
		}
	} while (!path.empty());
	return false;
}
