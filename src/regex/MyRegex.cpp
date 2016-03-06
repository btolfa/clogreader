#include "MyRegex.h"
#include "../tools/MyString.h"

MyRegex::MyRegex(const char* pattern, const size_t size) noexcept {}

bool MyRegex::regex_match(const char* input, const size_t size) const noexcept
{
	return false;
}

MyString MyRegex::simplify(MyString const& str)
{	
	auto result = str;
	auto result_ptr = result.data();	

	*result_ptr = str[0];
	result_ptr++;
	size_t newsize = 1;
	for (auto it = str.data() + 1; it != str.data() + str.size(); ++it) {
		if (!(*(result_ptr-1) == '*' && (*it == '*'))) {
			*result_ptr = *it;
			++newsize;
			++result_ptr;
		}
	}

	result.set_size(newsize);
	return result;
}
