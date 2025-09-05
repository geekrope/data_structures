#pragma once

#include <vector>

using namespace std;

vector<int> gcd_extended(int a, int b)
{
	if (a == 0)
	{
		return { b,0,1 };
	}

	auto gcd = gcd_extended(b % a, a);
	auto x = gcd[2] - b / a * gcd[1];
	auto y = gcd[1];
	return { gcd[0], x, y };
}
int mod_quotient(int a, int mod)
{
	auto gcd = gcd_extended(a, mod);

	if (gcd[0] != 1)
	{
		return -1;
	}
	else
	{
		return gcd[1];
	}
}