#pragma once

#include <cstdint>

// Based on https://swtch.com/~rsc/regexp/regexp1.html

class MyString;

class MyRegex
{
public:
	static MyString simplify(MyString const& str);
};

