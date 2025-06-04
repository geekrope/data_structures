#include <vector>
#include <algorithm>

using namespace std;

bool three_way_disjoint(vector<int> a, vector<int> b, vector<int> c)
{
	int ptr1 = 0, ptr2 = 0, ptr3 = 0;

	sort(a.begin(), a.end());
	sort(b.begin(), b.end());
	sort(c.begin(), c.end());

	while (ptr1 < a.size() && ptr2 < b.size() && ptr3 < c.size())
	{
		if (a[ptr1] == b[ptr2] && b[ptr2] == c[ptr3])
		{
			return false;
		}

		if (a[ptr1] < b[ptr2])
		{
			if (c[ptr3] < a[ptr1])
			{
				ptr3++;
			}
			else
			{
				ptr1++;
			}
		}
		else
		{
			if (c[ptr3] < b[ptr2])
			{
				ptr3++;
			}
			else
			{
				ptr2++;
			}
		}
	}

	return true;
}