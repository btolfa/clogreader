#include "MyRegex.h"
#include "../tools/MyString.h"

#include <algorithm>
#include <tuple>
#include <iterator>

/// Возможные типы состояний нашей NFA
enum class state_type : uint8_t
{
	This, ///< Ожидаем определённый символ
	Any,  ///< Ожидаем любой символ, соотвествует '?' во шаблоне
	Star, ///< Ожидаем любой количество символов, и после этого ожидаем определённый символ - '*a'
	EndOfLine, ///< Ожидаем конца входной строки
	Match ///< Если мы в этом состоянии, значит строка подходит под выражение
};

/// Структура для одного состояния
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

/// Количество активных символов + 1 для символа конца строки
auto count_active_symbol(const char* pattern, const size_t size) noexcept
{
	return std::count_if(pattern, pattern + size, [](const char ch)
	{
		return  ch != '*';
	});
}

/// Создаём все состояния из заданного шаблона
std::pair<State*, size_t> generate_states(const char* pattern, const size_t size)
{		
	// Для состояний нужно выделить количество активных символов в шаблоне + 1 для символа конца строки + 1 для Match состояния
	auto p_states = new (std::nothrow) State[count_active_symbol(pattern, size) + 1];

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
			if (*it == 0) {
				// Если последним символом был * - подходит строка с любым концом
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

class dynamic_array
{
public:
	using value_type = State *;
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

class StateOfFSM
{
public:
	using iterator = State *;

	StateOfFSM(size_t max_size) noexcept;

	// Перезапускаем FSM, если вернуло false то не можем запустить
	bool start(iterator first) noexcept;

	// Очистить контейнеры для следующего захода
	void clear() noexcept;

	// Если true - то мы нашли совпадение на текущем символе
	bool check(const char ch) noexcept;

	// Если true - мы нашли совпадение на конце входной строки
	bool check_eol() const noexcept;

	// Пустые ли контейнеры - часто это означает, что совпадения нет
	bool is_empty() const noexcept;

private:
	dynamic_array other;
	dynamic_array buffer;

	dynamic_array stars;

	dynamic_array new_stars;
};

auto count_stars(const char* pattern, const size_t size) noexcept
{
	return std::count_if(pattern, pattern + size, [](const char ch)
	{
		return  ch == '*';
	});
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

StateOfFSM::StateOfFSM(size_t max_size) noexcept : other{max_size}, buffer{max_size}, stars{max_size}, new_stars{max_size * 2} {}

void StateOfFSM::clear() noexcept
{
	other.clear();
	buffer.clear();
	stars.clear();
	new_stars.clear();
}

bool StateOfFSM::check_eol() const noexcept
{
	return std::any_of(other.begin(), other.end(), [](const auto p_state)
	{
		return p_state->type == state_type::EndOfLine || p_state->type == state_type::Match;
	});
}

bool StateOfFSM::is_empty() const noexcept
{
	return other.empty() && stars.empty();
}

bool StateOfFSM::start(iterator first) noexcept
{	
	// Если какой-то из контейнеров не готов, мы не можешь работать
	if (!(other && buffer && stars)) {
		return false;
	}

	if (first->is_star()) {
		stars.push_back(first);
	} else {
		other.push_back(first);
	}

	return true;
}

bool StateOfFSM::check(const char ch) noexcept
{
	new_stars.clear();
	buffer.clear();

	// Проверяем по не * состояниям
	for (auto it = other.begin(); it != other.end(); ++it) {
		auto b_do_next = false;
		
		switch ((*it)->type) {
		case state_type::This:
			if (ch == (*it)->symbol) {
				b_do_next = true;				
			}
			break;
		case state_type::Any:
			b_do_next = true;		
			break;			
		case state_type::EndOfLine: 
			break;
		case state_type::Match:
			return true;
		default: 
			break;
		}
		
		if (b_do_next) {
			if ((++(*it))->is_star()) {
				new_stars.push_back(*it);
			} else {
				buffer.push_back(*it);
			}
		}
	}

	// Теперь проверяем по * состояниям
	for (auto it = stars.begin(); it != stars.end(); ++it) {
		if (ch == (*it)->symbol) {
			// next
			auto next = *it + 1;
			if (next->is_star()) {
				new_stars.push_back(next);
			} else {
				buffer.push_back(next);
			}
		}
	}

	// Меняем местами старый и новые буферы для не * состояний
	swap(other, buffer);

	// Новые состояния * копируем их в stars только если они больше старых
	std::copy_if(new_stars.begin(), new_stars.end(), std::back_inserter(stars), [this](const auto it)
	{
		return stars.empty() || it > stars.back();
	});	

	return false;
}
