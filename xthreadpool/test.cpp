#include"xthreadpool.hpp"
#include<iostream>
int add(int m,int n)
{
	return m+n;
}
int main()
{
	x::XthreadPool test(1);
	int x=0,y=1;
	auto res=test.submit(add,x,y);
	auto ret=res.get();
	std::cout<<ret<<std::endl;
	return 0;
}
