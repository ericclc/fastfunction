#include "fastfunction.h"

#include <functional>
#include <iostream>

struct Foo
{
	Foo() {}

	int sum(int a, int b) const
	{ 
		return a + b;
	}
};

int sum(int a, int b)
{
	return a + b;
}

struct Functor
{
	int operator()(int i) const
	{
		return i + i;
	}
};

int main() try
{
	// store a free function
	FastFunction<int(int,int),16> testFree = sum;
	std::cout << "Free function: " << testFree(1,1) << std::endl;

	// store a lambda
	FastFunction<int()> testLambda = []() { return sum(2,2); };
	std::cout << "Lambda: " << testLambda() << std::endl;

	// store the result of a call to std::bind
	FastFunction<int()> testBindFree = std::bind(sum,3,3);
	std::cout << "Free bind: " << testBindFree() << std::endl;

	// store a call to a member function and object
	const Foo foo;
	using std::placeholders::_1;
	FastFunction<int(int)> testBindMember = std::bind(&Foo::sum, foo, _1, 4);
	std::cout << "Object & Member function: " << testBindMember(4) << std::endl;

	// store a call to a member function and object ptr
	FastFunction<int(int)> testBindMember2 = std::bind(&Foo::sum, &foo, _1, 5);
	std::cout << "Object pointer & Member function: " << testBindMember2(5) << std::endl;

	// store a call to a function object
	FastFunction<int(int)> testFunctor = Functor();
	std::cout << "Functor: " << testFunctor(6) << std::endl;

	return 0;
}
catch (std::exception exception)
{
	std::cout << exception.what() << std::endl;
}
