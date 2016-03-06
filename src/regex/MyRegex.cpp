#include "MyRegex.h"

#include "State.h"
#include "StateOfFSM.h"

#include <tuple>

/// Количество активных символов + 1 для символа конца строки
auto count_active_symbol(const char* pattern, const size_t size) noexcept
{
	return std::count_if(pattern, pattern + size, [](const char ch)
	{
		return  ch != '*';
	});
}

/// Количество * в шаблоне
auto count_stars(const char* pattern, const size_t size) noexcept
{
	return std::count_if(pattern, pattern + size, [](const char ch)
	{
		return  ch == '*';
	});
}


/// Создаём все состояния из заданного шаблона
std::pair<State*, size_t> generate_states(const char* pattern, const size_t size)
{		
	// Для состояний нужно выделить количество активных символов в шаблоне + 1 для символа конца строки + 1 для Match состояния
	auto p_states = new (std::nothrow) State[count_active_symbol(pattern, size) + 2];

	// Если нет памяти, то ничего делать нельзя
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
			*p_next_state++ = State{ state_type::Star, *it };			
			b_is_star = false;
		} else {
			if (*it == '?') {
				*p_next_state++ = State{ state_type::Any}; 			
 			} else {
				*p_next_state++ = State{ state_type::This, *it };
 			}
		}
	}

	if (b_is_star) {
		*p_next_state++ = State{ state_type::Match };
	} else {
		*p_next_state++ = State{ state_type::EndOfLine };
		*p_next_state++ = State{ state_type::Match };
	}

	return std::make_pair(p_states, (p_next_state - p_states)/sizeof(State));
}

MyRegex::MyRegex(const char* pattern, const size_t size) noexcept {
	auto simplified = simplify(pattern, size);

	// Если на предыдущем шаге не выделили памяти, то тут нечего делать.
	if (! simplified.first) {
		return;
	}

	std::tie(p_states, states_size) = generate_states(simplified.first, simplified.second);
	p_sof = new (std::nothrow) StateOfFSM(count_stars(pattern, size) + 1);

	delete[] simplified.first;
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
		if (p_sof->check(*it)) {
			return true;
		}

		if (p_sof->is_empty()) {
			return false;
		}
	}

	return p_sof->check_eol();
}

std::pair<char *, size_t> MyRegex::simplify(const char* pattern, const size_t size) noexcept
{	
	// Если строка пустая, то нечего делать
	if (size == 0) {
		return{ nullptr, 0 };
	}

	// Выделяем память под новую строку
	char * result = new (std::nothrow) char[size];

	// Если память не выделилась, то нам нечего делать
	if (! result) {
		return{ nullptr, 0 };
	}	

	auto out_ptr = result;
	*out_ptr = *pattern;
	for (auto ptr = pattern + 1; ptr != pattern + size; ++ptr) {
		switch(*ptr) {
		case '?':
			if (*out_ptr == '*') {
				// Ставим ? перед *
				*out_ptr = '?';
				*(++out_ptr) = '*';
			}
			else {
				*(++out_ptr) = '?';
			}
			break;
		case '*':
			if (*out_ptr != '*') {
				*(++out_ptr) = '*';
			}
			break;
		default:
			*(++out_ptr) = *ptr;
			break;
		}
	}

	return{ result, out_ptr - result + 1 };
}

MyRegex::operator bool() const noexcept{
	return p_states && p_sof && *p_sof;
}
