#include "MyRegex.h"
#include "../tools/MyString.h"

#include <algorithm>

/// ��������� ���� ��������� ����� NFA
enum class state_type : uint8_t
{
	This, ///< ������� ����������� ������
	Any,  ///< ������� ����� ������, ������������ '?' �� �������
	Star, ///< ������� ����� ���������� ��������, � ����� ����� ������� ����������� ������ - '*a'
	EndOfLine, ///< ������� ����� ������� ������
	Match ///< ���� �� � ���� ���������, ������ ������ �������� ��� ���������
};

/// ��������� ��� ������ ���������
struct State
{
	State() noexcept {};
	State(const state_type type, char symbol = 0) : type{ type }, symbol{ symbol } {}

	bool is_star() const noexcept
	{
		return type == state_type::Star;
	}

	state_type type;
	char symbol;
};

/// ���������� �������� �������� + 1 ��� ������� ����� ������
auto count_active_symbol(const char* pattern, const size_t size) noexcept
{
	return std::count_if(pattern, pattern + size, [](const char ch)
	{
		return  ch != '*';
	});
}

/// ������ ��� ��������� �� ��������� �������
State * generate_states(const char* pattern, const size_t size)
{		
	// ��� ��������� ����� �������� ���������� �������� �������� � ������� + 1 ��� ������� ����� ������ + 1 ��� Match ���������
	auto p_states = new (std::nothrow) State[count_active_symbol(pattern, size) + 1];

	// ���� ��� ������, �� ������ ������ ������
	if (! p_states) {
		return nullptr;
	}

	auto b_is_star = false;
	auto p_next_state = p_states;
	for (auto it = pattern; it != pattern + size; ++it) {
		if (*it == '*') {
			b_is_star = true;
			continue;
		}

		if (b_is_star) {
			if (*it == 0) {
				// ���� ��������� �������� ��� * - �������� ������ � ����� ������
				*p_next_state++ = State{ state_type::Match };				
			} else {				
				*p_next_state++ = State{ state_type::Star, *it };				
			}
			b_is_star = false;
		} else {
			if (*it == '?') {
				*p_next_state++ = State{ state_type::Any};
 			} else if (*it == 0) {
				*p_next_state++ = State{ state_type::EndOfLine };
				*p_next_state++ = State{ state_type::Match };
 			} else {
				*p_next_state++ = State{ state_type::This, *it };
 			}
		}
	}

	return p_states;
}

MyRegex::MyRegex(const char* pattern, const size_t size) noexcept : p_states{ generate_states(pattern, size) } {

}

MyRegex::~MyRegex() noexcept
{
	delete[] p_states;
}

bool MyRegex::regex_match(const char* input, const size_t size) const noexcept
{
	if (! p_states) {
		return false;
	}

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
