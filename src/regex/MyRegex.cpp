#include "MyRegex.h"
#include "../tools/MyString.h"

#include "State.h"
#include "StateOfFSM.h"

#include <tuple>

/// ���������� �������� �������� + 1 ��� ������� ����� ������
auto count_active_symbol(const char* pattern, const size_t size) noexcept
{
	return std::count_if(pattern, pattern + size, [](const char ch)
	{
		return  ch != '*';
	});
}

/// ���������� * � �������
auto count_stars(const char* pattern, const size_t size) noexcept
{
	return std::count_if(pattern, pattern + size, [](const char ch)
	{
		return  ch == '*';
	});
}


/// ������ ��� ��������� �� ��������� �������
std::pair<State*, size_t> generate_states(const char* pattern, const size_t size)
{		
	// ��� ��������� ����� �������� ���������� �������� �������� � ������� + 1 ��� ������� ����� ������ + 1 ��� Match ���������
	auto p_states = new (std::nothrow) State[count_active_symbol(pattern, size) + 1];

	// ���� ��� ������, �� ������ ������ ������
	if (! p_states) {
		return std::make_pair(nullptr, 0);
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

	return std::make_pair(p_states, (p_next_state - p_states)/sizeof(State));
}

MyRegex::MyRegex(const char* pattern, const size_t size) noexcept {
	std::tie(p_states, states_size) = generate_states(pattern, size);
	p_sof = new (std::nothrow) StateOfFSM(count_stars(pattern, size) + 1);
}

MyRegex::~MyRegex() noexcept
{
	delete[] p_states;
	delete p_sof;
}

bool MyRegex::regex_match(const char* input, const size_t size) noexcept
{
	if (! p_states || ! p_sof) {
		return false;
	}

	p_sof->clear();
	if (! p_sof->start(p_states)) {
		return false;
	}

	for (auto it = input; it != input + size; ++it) {
		if (*it != 0) {
			if (p_sof->check(*it)) {
				return true;
			}
		} else {
			return p_sof->check_eol();
		}

		if (p_sof->is_empty()) {
			return false;
		}
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