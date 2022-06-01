#pragma once

namespace free {

inline int add(int lhs, int rhs)
{
	return lhs + rhs;
}

inline int sub(int lhs, int rhs)
{
	return lhs - rhs;
}

inline void foo() {}

}

int main()
{
	auto u = free::add(3, 5);
	return 0;
}