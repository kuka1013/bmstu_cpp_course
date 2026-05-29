#pragma once

#include <compare>
#include <cstddef>
#include <iterator>
#include <ostream>
#include "abstract_iterator.h"

namespace bmstu
{
template <typename T>
class list
{
	struct node
	{
		node() = default;

		node(node* prev, const T& value, node* next)
			: next_node_(next), prev_node_(prev), value_(value)
		{
		}

		T value_;
		node* next_node_ = nullptr;
		node* prev_node_ = nullptr;
	};

   public:
	template <typename ValueType, typename NodeT>
	struct basic_iterator
		: public abstract_iterator<basic_iterator<ValueType, NodeT>,
								   ValueType,
								   std::bidirectional_iterator_tag>
	{
		using difference_type = typename abstract_iterator<
			basic_iterator<ValueType, NodeT>,
			ValueType,
			std::bidirectional_iterator_tag>::difference_type;

		NodeT* current;

		basic_iterator() : current(nullptr) {}
		basic_iterator(NodeT* node) : current(node) {}

		template <typename V, typename N>
		basic_iterator(const basic_iterator<V, N>& other)
			: current(other.current)
		{
		}

		basic_iterator& operator++() override
		{
			current = current->next_node_;
			return *this;
		}

		basic_iterator& operator--() override
		{
			current = current->prev_node_;
			return *this;
		}

		basic_iterator operator++(int) override
		{
			basic_iterator temp = *this;
			current = current->next_node_;
			return temp;
		}

		basic_iterator operator--(int) override
		{
			basic_iterator temp = *this;
			current = current->prev_node_;
			return temp;
		}

		basic_iterator& operator+=(const difference_type& n) override
		{
			difference_type steps = n;
			if (steps >= 0)
			{
				while (steps--)
				{
					current = current->next_node_;
				}
			}
			else
			{
				while (steps++)
				{
					current = current->prev_node_;
				}
			}
			return *this;
		}

		basic_iterator& operator-=(const difference_type& n) override
		{
			return *this += (-n);
		}

		basic_iterator operator+(const difference_type& n) const override
		{
			basic_iterator temp = *this;
			temp += n;
			return temp;
		}

		basic_iterator operator-(const difference_type& n) const override
		{
			basic_iterator temp = *this;
			temp -= n;
			return temp;
		}

		typename abstract_iterator<basic_iterator<ValueType, NodeT>,
								   ValueType,
								   std::bidirectional_iterator_tag>::reference
		operator*() const override
		{
			return current->value_;
		}

		typename abstract_iterator<basic_iterator<ValueType, NodeT>,
								   ValueType,
								   std::bidirectional_iterator_tag>::pointer
		operator->() const override
		{
			return &(current->value_);
		}

		bool operator==(const basic_iterator& other) const override
		{
			return current == other.current;
		}

		bool operator!=(const basic_iterator& other) const override
		{
			return current != other.current;
		}

		template <typename V, typename N>
		bool operator==(const basic_iterator<V, N>& other) const
		{
			return current == other.current;
		}

		template <typename V, typename N>
		bool operator!=(const basic_iterator<V, N>& other) const
		{
			return current != other.current;
		}

		explicit operator bool() const override { return current != nullptr; }

		typename abstract_iterator<
			basic_iterator<ValueType, NodeT>,
			ValueType,
			std::bidirectional_iterator_tag>::difference_type
		operator-(const basic_iterator& other) const override
		{
			difference_type dist = 0;
			NodeT* temp = other.current;

			while (temp != current && temp != nullptr)
			{
				temp = temp->next_node_;
				dist++;
			}
			if (temp == nullptr)
			{
				dist = 0;
				temp = other.current;
				while (temp != current && temp != nullptr)
				{
					temp = temp->prev_node_;
					dist++;
				}
			}
			return dist;
		}
	};

	using iterator = basic_iterator<T, node>;
	using const_iterator = basic_iterator<const T, const node>;

	list()
	{
		head_.next_node_ = &tail_;
		tail_.prev_node_ = &head_;
		size_ = 0;
	}

	template <typename it>
	list(it begin, it end) : list()
	{
		for (auto current_it = begin; current_it != end; ++current_it)
		{
			push_back(*current_it);
		}
	}

	list(std::initializer_list<T> values) : list()
	{
		for (const auto& value : values)
		{
			push_back(value);
		}
	}

	list(const list& other) : list()
	{
		for (const auto& value : other)
		{
			push_back(value);
		}
	}

	list(list&& other) noexcept : list() { this->swap(other); }

#pragma endregion
#pragma region pushs

	template <typename Type>
	void push_back(const Type& value)
	{
		insert(end(), value);
	}

	template <typename Type>
	void push_front(const Type& value)
	{
		insert(begin(), value);
	}

#pragma endregion

	bool empty() const noexcept { return (size_ == 0u); }

	~list() { clear(); }

	void clear()
	{
		node* curr = head_.next_node_;
		while (curr != &tail_)
		{
			node* next = curr->next_node_;
			delete curr;
			curr = next;
		}
		head_.next_node_ = &tail_;
		tail_.prev_node_ = &head_;
		size_ = 0;
	}

	size_t size() const { return size_; }

	void swap(list& other) noexcept
	{
		std::swap(size_, other.size_);
		std::swap(head_.next_node_, other.head_.next_node_);
		std::swap(tail_.prev_node_, other.tail_.prev_node_);

		if (size_ == 0)
		{
			head_.next_node_ = &tail_;
			tail_.prev_node_ = &head_;
		}
		else
		{
			head_.next_node_->prev_node_ = &head_;
			tail_.prev_node_->next_node_ = &tail_;
		}

		if (other.size_ == 0)
		{
			other.head_.next_node_ = &other.tail_;
			other.tail_.prev_node_ = &other.head_;
		}
		else
		{
			other.head_.next_node_->prev_node_ = &other.head_;
			other.tail_.prev_node_->next_node_ = &other.tail_;
		}
	}

	friend void swap(list& l, list& r) { l.swap(r); }

#pragma region iterators

	iterator begin() noexcept { return iterator{head_.next_node_}; }

	iterator end() noexcept { return iterator{&tail_}; }

	const_iterator begin() const noexcept
	{
		return const_iterator{head_.next_node_};
	}

	const_iterator end() const noexcept { return const_iterator{&tail_}; }

	const_iterator cbegin() const noexcept
	{
		return const_iterator{head_.next_node_};
	}

	const_iterator cend() const noexcept { return const_iterator{&tail_}; }

#pragma endregion

	const T operator[](size_t pos) const
	{
		auto it = begin();
		for (size_t i = 0; i < pos; ++i)
		{
			++it;
		}
		return *it;
	}

	T& operator[](size_t pos)
	{
		auto it = begin();
		for (size_t i = 0; i < pos; ++i)
		{
			++it;
		}
		return *it;
	}

	friend bool operator==(const list& l, const list& r)
	{
		if (l.size() != r.size())
		{
			return false;
		}

		auto it_l = l.begin();
		auto it_r = r.begin();

		while (it_l != l.end())
		{
			if (*it_l != *it_r)
			{
				return false;
			}
			++it_l;
			++it_r;
		}
		return true;
	}

	friend bool operator!=(const list& l, const list& r) { return !(r == l); }

	friend auto operator<=>(const list& lhs, const list& rhs)
	{
		if (lexicographical_compare_(lhs, rhs))
		{
			return std::strong_ordering::less;
		}
		if (lexicographical_compare_(rhs, lhs))
		{
			return std::strong_ordering::greater;
		}
		return std::strong_ordering::equivalent;
	}

	friend std::ostream& operator<<(std::ostream& os, const list& other)
	{
		os << "{";
		bool first = true;

		for (const auto& value : other)
		{
			if (!first)
			{
				os << ", ";
			}
			os << value;
			first = false;
		}
		os << "}";
		return os;
	}

	iterator insert(const_iterator pos, const T& value)
	{
		node* curr = const_cast<node*>(pos.current);
		node* prev = curr->prev_node_;

		node* new_node = new node(prev, value, curr);
		prev->next_node_ = new_node;
		curr->prev_node_ = new_node;

		++size_;

		return iterator{new_node};
	}

   private:
	static bool lexicographical_compare_(const list<T>& l, const list<T>& r)
	{
		auto it_l = l.begin();
		auto it_r = r.begin();

		while (it_l != l.end() && it_r != r.end())
		{
			if (*it_l < *it_r)
			{
				return true;
			}
			if (*it_r < *it_l)
			{
				return false;
			}
			++it_l;
			++it_r;
		}

		return (it_l == l.end() && it_r != r.end());
	}

	size_t size_ = 0;
	node tail_;
	node head_;
};
}  // namespace bmstu