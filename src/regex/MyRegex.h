#pragma once

#include <cstdint>

// Based on https://swtch.com/~rsc/regexp/regexp1.html

class MyString;
struct State;

class MyRegex
{
public:
	MyRegex(const char * pattern, const size_t size) noexcept;
	bool regex_match(const char * input, const size_t size) const noexcept;

	static MyString simplify(MyString const& str);

private:
	State * states;	
};

