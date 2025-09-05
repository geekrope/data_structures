#pragma once

#include <algorithm>
#include <list>
#include <cassert>
#include <iterator>
#include <stdexcept>
#include <vector>

namespace heap
{
	template<typename T, typename comp = std::less<T>>
	class interface
	{
	private:
		using container = std::vector<T>;
		static constexpr comp precede = comp{};

	public:
		static size_t parent_index(size_t index)
		{
			assert(index > 0);
			return (index - 1) / 2;
		}
		static size_t left_index(size_t index)
		{
			return 2 * index + 1;
		}
		static size_t right_index(size_t index)
		{
			return 2 * index + 2;
		}

		static void swap(container& data, size_t x, size_t y)
		{
			auto temp = data[x];
			data[x] = data[y];
			data[y] = temp;
		}

		static void bubble_up(container& data, size_t start)
		{
			size_t current = start;

			//current>0 is equivalent to current being a child of some element
			while (current > 0)
			{
				auto parent = parent_index(current);

				if (precede(data[current], data[parent]))
				{
					swap(data, parent, current);
					current = parent;
				}
				else
				{
					break;
				}
			}
		}
		static void bubble_down(container& data, size_t end)
		{
			auto current = size_t{};

			while (true)
			{
				auto left = left_index(current);
				auto right = right_index(current);

				if (right < end)
				{
					if (precede(data[left], data[current]) && precede(data[left], data[right]))
					{
						swap(data, current, left);
						current = left;
					}
					else if (precede(data[right], data[current]))
					{
						swap(data, current, right);
						current = right;
					}
					else
					{
						break;
					}
				}
				else if (left < end && precede(data[left], data[current]))
				{
					swap(data, current, left);
					current = left;
				}
				else
				{
					break;
				}
			}
		}

		static void make_valid(container& data)
		{
			for (size_t index = 0; index < data.size(); index++)
			{
				bubble_up(data, index);
			}
		}
		static void sort(container& data)
		{
			for (size_t end = data.size() - 1; end > 0; end--)
			{
				swap(data, 0, end);
				bubble_down(data, end);
			}
		}

		static void insert(container& data, const T& value)
		{
			data.push_back(value);
			bubble_up(data, data.size() - 1);
		}
		static void replace_top(container& data, const T& value)
		{
			if (data.empty())
			{
				insert(data, value);
			}
			else
			{
				data[0] = value;
				bubble_down(data, data.size());
			}
		}
		static T remove(container& data)
		{
			if (data.empty())
			{
				throw std::invalid_argument{ "The data is empty" };
			}

			auto root = data[0];

			data[0] = data[data.size() - 1];
			data.pop_back();
			bubble_down(data, data.size());

			return root;
		}
	};

	template<typename T, typename comp = std::less<T>>
	class heap
	{
	private:
		std::vector<T>& data;
		using interface = interface<T, comp>;
	public:
		heap(std::vector<T>& data)
			:data(data)
		{
			interface::make_valid(data);
		}

		void insert(const T& value)
		{
			interface::insert(data, value);
		}
		void replace_top(const T& value)
		{
			interface::replace_top(data, value);
		}
		T remove()
		{
			return interface::remove(data);
		}

		bool empty()
		{
			return data.empty();
		}
	};

	template<typename T, typename comp = std::less<T>>
	class heap_wrapper
	{
	private:
		using element_type = std::reference_wrapper<T>;
		struct comp_wrapper
		{
			static constexpr comp precede = comp{};
			constexpr bool operator()(element_type left, element_type right) const
			{
				return precede(left.get(), right.get());
			}
		};
		std::list<T>& original_data;
		std::vector<element_type> data;
		using interface = interface<element_type, comp_wrapper>;
	public:
		heap_wrapper() = default;
		heap_wrapper(std::list<T>& original_data)
			:original_data(original_data)
		{
			data.reserve(original_data.size());
			std::transform(original_data.begin(), original_data.end(),
				std::back_insert_iterator<std::vector<element_type>>(data), 
				[](auto& item) { return std::ref(item); });
			interface::make_valid(data);
		}

		void insert(const T& value)
		{
			original_data.push_back(value);
			interface::insert(data, std::ref(original_data.back()));
		}
		void replace_top(const T& value)
		{
			if (data.empty())
			{
				insert(value);
			}
			else
			{
				data[0].get() = value;
				interface::bubble_down(data, data.size());
			}
		}
		T remove()
		{
			return interface::remove(data);
		}

		bool empty()
		{
			return data.empty();
		}
	};

	template<typename T, typename comp = std::less<T>>
	class self_contained_heap : public heap<T, comp>
	{
	private:
		std::vector<T> data;
	public:
		self_contained_heap()
			: heap<T, comp>(data)
		{

		}
	};

	template<typename K, typename V, typename comp = std::less<K>>
	class priority_queue
	{
	private:
		using element_type = std::pair<K, V>;
		struct key_comparer
		{
			static constexpr comp precede = comp{};
			constexpr bool operator()(element_type left, element_type right) const
			{
				return precede(left.first, right.first);
			}
		};
		heap_wrapper<element_type, key_comparer> heap;
	public:
		priority_queue() = default;
		priority_queue(std::list<element_type>& data)
			: heap(data)
		{

		}

		void enqueue(const K& key, const V& value)
		{
			heap.insert({ key, value });
		}
		element_type dequeue()
		{
			return heap.remove();
		}
		bool empty()
		{
			return heap.empty();
		}
	};

	template<typename T, typename comp = std::less<T>>
	void heap_sort(std::vector<T>& v)
	{
		interface<T, comp>::make_valid(v);
		interface<T, comp>::sort(v);
	}
}