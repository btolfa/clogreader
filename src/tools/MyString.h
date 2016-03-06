#pragma once

#include <new>
#include <cstring>

class MyString
{
public:
	MyString() {}

	MyString(const char * str, const size_t size) noexcept : size_{ size } {
		data_ = new (std::nothrow) char[size_];
		if (data_) {
			std::memcpy(data_, str, size_);
		}
	}

	~MyString() noexcept
	{
		if (data_)
		{
			delete[] data_;
		}
	}

	MyString(const MyString& other) noexcept : size_(other.size_)
	{
		data_ = new (std::nothrow) char[size_];
		if (data_)
		{
			std::memcpy(data_, other.data_, size_);
		}
	}

	MyString(MyString&& other) noexcept
		: data_(other.data_),
		  size_(other.size_)
	{
		other.data_ = nullptr;
		other.size_ = 0;
	}

	MyString& operator=(const MyString& other) noexcept
	{
		MyString tmp (other);
		*this = std::move(tmp);
		return *this;
	}

	MyString& operator= (MyString&& other) noexcept
	{
		if (data_)
		{
			delete[] data_;			
		}
		
		data_ = other.data_;
		size_ = other.size_;

		other.data_ = nullptr;
		other.size_ = 0;

		return *this;
	}

	explicit operator bool() const noexcept
	{
		return data_ != nullptr;
	}

	char * c_str() noexcept
	{
		return data_;
	}

	char * data() noexcept {
		return data_;
	}

	const char * c_str() const noexcept
	{
		return data_;
	}

	const char * data() const noexcept {
		return data_;
	}

	size_t lenght() const noexcept
	{
		return size_;
	}

	size_t size() const noexcept
	{
		return size_;
	}

	char operator[](size_t pos) const noexcept{
		return data_[pos];
	}

	void set_size(const size_t size) noexcept {
		size_ = size;
	}

private:
	char * data_ {nullptr};
	size_t size_ {0};
};