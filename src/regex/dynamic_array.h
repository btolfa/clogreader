#pragma once

#include <utility>

template <typename T>
class dynamic_array
{
public:
	using value_type = typename T;
	using pointer = value_type *;

	dynamic_array(size_t max_size) noexcept
	{
		data_ = new (std::nothrow) value_type[max_size];
	}

	~dynamic_array() noexcept
	{
		delete[] data_;
	}

	explicit operator bool() const noexcept
	{
		return data_ != nullptr;
	}

	void push_back(value_type const& value) noexcept
	{
		data_[size_++] = value;
	}

	void clear() noexcept
	{
		size_ = 0;
	}

	pointer begin() noexcept
	{
		return data_;
	}

	pointer end() noexcept
	{
		return data_ + size_;
	}

	pointer begin() const noexcept
	{
		return data_;
	}

	pointer end() const noexcept
	{
		return data_ + size_;
	}

	bool empty() const noexcept {
		return size_ == 0;
	}

	value_type back() const noexcept {
		return data_[size_ - 1];
	}

	friend void swap(dynamic_array & lhs, dynamic_array & rhs) noexcept
	{
		std::swap(lhs.data_, rhs.data_);
		std::swap(lhs.size_, rhs.size_);
	}

private:

	value_type * data_{ nullptr };
	size_t size_{ 0 };
};