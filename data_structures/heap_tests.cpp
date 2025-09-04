#include <functional>
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <string>
#include <vector>

#include "heap.h"

using namespace std;

static string vec_to_string(const vector<int>& v)
{
	ostringstream os;
	os << "{";
	for (size_t i = 0; i < v.size(); ++i)
	{
		if (i) os << ", ";
		os << v[i];
	}
	os << "}";
	return os.str();
}

static void expect_eq_int(int a, int b, const string& msg)
{
	if (a != b)
		throw runtime_error(msg + " — got: " + to_string(a) + " expected: " + to_string(b));
}

static void expect_eq_vec(const vector<int>& a, const vector<int>& b, const string& msg)
{
	if (a != b)
		throw runtime_error(msg + " — got: " + vec_to_string(a) + " expected: " + vec_to_string(b));
}

bool run_tests()
{
	int passed = 0, failed = 0;

	auto run = [&](const string& name, const function<void()>& fn)
		{
			try
			{
				fn();
				cout << name << " — PASS\n";
				++passed;
			}
			catch (const exception& e)
			{
				cout << name << " — FAIL: " << e.what() << "\n";
				++failed;
			}
			catch (...)
			{
				cout << name << " — FAIL: unknown\n";
				++failed;
			}
		};

	run("parent/left/right indices", []()
		{
			expect_eq_int(heap::interface<int>::parent_index(1), 0, "parent_index(1)");
			expect_eq_int(heap::interface<int>::parent_index(2), 0, "parent_index(2)");
			expect_eq_int(heap::interface<int>::left_index(0), 1, "left_index(0)");
			expect_eq_int(heap::interface<int>::right_index(0), 2, "right_index(0)");
		});

	run("insert/remove order (min-heap -> ascending removals)", []()
		{
			vector<int> v;
			heap::heap<int> h{ v };
			h.insert(5);
			h.insert(3);
			h.insert(8);
			h.insert(1);

			vector<int> removed;
			while (!h.empty())
				removed.push_back(h.remove());

			vector<int> expected = { 1,3,5,8 };
			expect_eq_vec(removed, expected, "removal sequence");
		});

	run("make_valid and heap_sort (heap_sort for min-heap -> descending result)", []()
		{
			vector<int> v = { 5,1,4,2,3 };
			heap::heap_sort(v); // for min-heap implementation this produces descending order
			vector<int> expected = { 5,4,3,2,1 };
			expect_eq_vec(v, expected, "heap_sort result (expected descending for min-heap)");
		});

	run("replace_top behavior", []()
		{
			vector<int> v = { 4,7,9 };
			heap::heap<int> h{ v }; // makes valid
			h.replace_top(1); // new minimum
			int first = h.remove();
			expect_eq_int(first, 1, "replace_top did not set new minimum at top");
		});

	run("remove throws on empty", []()
		{
			vector<int> v;
			heap::heap<int> h{ v };
			try
			{
				h.remove();
				throw runtime_error("remove() did not throw on empty");
			}
			catch (const invalid_argument&)
			{
				// expected
			}
		});

	cout << "\nSummary: passed = " << passed << ", failed = " << failed << "\n";
	return failed == 0;
}