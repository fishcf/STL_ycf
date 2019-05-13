#include <iostream>

template <typename T> T lt(T val1, T val2)
{
	return val1 < val2 ? val1: val2;
}

template<typename T> T max(T val1, T val2)
{
	return val1 > val2 ? val1 : val2;
}
