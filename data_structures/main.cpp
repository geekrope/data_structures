#include <algorithm>
#include <chrono>
#include <iostream>
#include <vector>
#include <random>
#include <limits>

#include "heap.h"

using namespace std;
extern bool run_tests();

void print(const vector<long long>& v)
{
	cout << "[";
	for (auto& i : v)
	{
		cout << i << ",";
	}
	cout << "]";
}

int main()
{
	if (!run_tests())
	{
		throw std::runtime_error{ "One or more tests failed" };
	}

	constexpr auto elements_count = 100000;
	constexpr auto single_cap = 2000;

	using heap_element = std::vector<long long>;
	vector<heap_element> massive(elements_count);

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<long long> dist(std::numeric_limits<long long>::min(), std::numeric_limits<long long>::max());

	for (auto i = 0; i < elements_count; ++i)
	{
		heap_element value(single_cap);
		for (int j = 0; j < single_cap; ++j)
		{
			value[j] = dist(gen);
		}

		massive[i] = value;
	}

	auto start_point = std::chrono::steady_clock::now();
	heap::heap_wrapper<heap_element> min_heap_wrapper{ massive };
	std::cout << endl << "heap wrapper: " <<
		std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start_point).count() << " ms" << endl;


	/*std::cout << "Wrapper Ascending: ";
	for (auto i = 0; i < 100; ++i)
	{
		print(min_heap_wrapper.remove());
		std::cout << endl;
	}*/

	start_point = std::chrono::steady_clock::now();
	heap::heap<heap_element> min_heap{ massive };
	std::cout << endl << "heap: " <<
		std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start_point).count() << " ms" << endl;

	/*std::cout << "Ascending: ";
	for (auto i = 0; i < 100; ++i)
	{
		print(min_heap_wrapper.remove());
		std::cout << endl;
	}*/
}