#pragma once

#include <new>
#include <cstring>

class MyString
{
public:
	MyString() {}

	MyString(const char * str, const size_t size) : size_{ size } {
		data_ = new (std::nothrow) char[size_];
	}

	~MyString() noexcept
	{
		if (data_)
		{
			delete[] data_;
		}
	}

	MyString(const MyString& other) : size_(other.size_)
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

	MyString& operator=(const MyString& other)
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

	size_t lenght() const noexcept
	{
		return size_;
	}

private:
	char * data_ {nullptr};
	size_t size_ {0};
};