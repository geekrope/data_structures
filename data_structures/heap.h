#pragma once
#include <vector>

namespace Heap
{
	template<typename T, typename comp = std::less<T>>
	class Heap
	{
	private:
		std::vector<T> data;
		static constexpr comp precede = comp{};

		int parent(int index)
		{
			return (index - 1) / 2;
		}
		int left(int index)
		{
			return 2 * index + 1;
		}
		int right(int index)
		{
			return 2 * index + 2;
		}

		void swap(int x, int y)
		{
			auto temp = data[x];
			data[x] = data[y];
			data[y] = temp;
		}

		void bubble_up(int index)
		{
			int parent = this->parent(index);

			if (parent < 0)
			{
				return;
			}

			if (precede(data[index], data[parent]))
			{
				swap(parent, index);
				bubble_up(parent);
			}
		}
		void bubble_down(int index)
		{
			int left = this->left(index);
			int right = this->right(index);

			if (right < data.size())
			{
				if (precede(data[left], data[index]) && precede(data[left], data[right]))
				{
					swap(index, left);
					bubble_down(left);
				}
				else if (precede(data[right], data[index]))
				{
					swap(index, right);
					bubble_down(right);
				}
			}
			else if (left < data.size() && precede(data[left], data[index]))
			{
				swap(index, left);
				bubble_down(left);
			}
		}

	public:
		void reserve(size_t capacity)
		{
			data.reserve(capacity);
		}
		void insert(int value)
		{
			data.push_back(value);
			bubble_up(data.size() - 1);
		}
		int remove()
		{
			auto root = data[0];
			data[0] = data[data.size() - 1];
			data.pop_back();
			bubble_down(0);
			return root;
		}
		int size()
		{
			return data.size();
		}
		bool is_empty()
		{
			return data.size() == 0;
		}
	};

	template<typename T>
	void swap(std::vector<T>& v, int x, int y)
	{
		auto temp = v[x];
		v[x] = v[y];
		v[y] = temp;
	}

	template<typename T, typename comp = std::less<T>>
	void heap_sort(std::vector<T>& v)
	{
		constexpr auto precede = comp{};
		for (int index = 0; index < v.size(); index++)
		{
			auto current = index;

			while (true)
			{
				auto parent = (current - 1) / 2;

				if (parent < 0)
				{
					break;
				}

				if (precede(v[current], v[parent]))
				{
					swap(v, current, parent);
					current = parent;
				}
				else
				{
					break;
				}
			}
		}

		for (int end = v.size() - 1; end > 0; end--)
		{
			swap(v, 0, end);

			auto current = 0;

			while (true)
			{
				int left = current * 2 + 1;
				int right = current * 2 + 2;

				if (right < end)
				{
					if (precede(v[left], v[current]) && precede(v[left], v[right]))
					{
						swap(v, current, left);
						current = left;
					}
					else if (precede(v[right], v[current]))
					{
						swap(v, current, right);
						current = right;
					}
					else
					{
						break;
					}
				}
				else if (left < end && precede(v[left], v[current]))
				{
					swap(v, current, left);
					current = left;
				}
				else
				{
					break;
				}
			}
		}
	}
}