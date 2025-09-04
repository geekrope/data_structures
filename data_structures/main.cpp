#include <iostream>
#include <algorithm>
#include <vector>

#include "heap.h"

using namespace std;

int main()
{
	vector<int> massive = { -861, 541, 816, -656, -125, 201, 5, -464, 879, -483, 34, 977, -726, 0, -482, -210, -338, 49, -281, 135, 59, -941, -609, -796, -405, -714, 238, -451, 671, 196, 624, 191, 256, -181, -68, 252, 712, -779, 473, 405, 454, -82, 372, 604, 835, -249, 146, -512, 894, 888 };
	Heap::Heap<int> min_heap{};
	Heap::Heap<int, greater<int>> max_heap{};
	for (const auto item : massive)
	{
		min_heap.insert(item);
		max_heap.insert(item);
	}
	cout << "Ascending: ";
	while (!min_heap.is_empty())
	{
		cout << min_heap.remove() << " ";
	}
	cout << endl << "Descending: ";
	while (!max_heap.is_empty())
	{
		cout << max_heap.remove() << " ";
	}
}