#pragma once

#include "State.h"
#include "dynamic_array.h"
#include <algorithm>
#include <iterator>


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

	explicit operator bool() const noexcept {
		return other && buffer && stars && tmp;
	}

private:
	dynamic_array<iterator> other;
	dynamic_array<iterator> buffer;

	dynamic_array<iterator> stars;

	dynamic_array<iterator> tmp;
};

inline StateOfFSM::StateOfFSM(size_t max_size) noexcept : other{ max_size }, buffer{ max_size }, stars{ max_size }, tmp{ max_size * 2 } {}

inline void StateOfFSM::clear() noexcept
{
	other.clear();
	buffer.clear();
	stars.clear();
	tmp.clear();
}

inline bool StateOfFSM::check_eol() const noexcept
{
	return std::any_of(other.begin(), other.end(), [](const auto p_state)
	{
		return p_state->type == state_type::EndOfLine || p_state->type == state_type::Match;
	});
}

inline bool StateOfFSM::is_empty() const noexcept
{
	return other.empty() && stars.empty();
}

inline bool StateOfFSM::start(iterator first) noexcept
{
	// Если какой-то из контейнеров не готов, мы не можешь работать
	if (!(other && buffer && stars)) {
		return false;
	}

	if (first->is_star()) {
		stars.push_back(first);
	}
	else {
		other.push_back(first);
	}

	return true;
}

inline bool StateOfFSM::check(const char ch) noexcept
{
	tmp.clear();
	buffer.clear();

	// Если есть совпадения выходим
	if (std::any_of(other.begin(), other.end(),
	                [](const auto p_state) {
		                return p_state->is_match();
	                })) {
		return true;
	}

	// Копируем изменившиеся состояния из other в tmp
	std::copy_if(other.begin(), other.end(), std::back_inserter(tmp),
	             [ch](const auto p_state) {
		             return p_state->is_any() || p_state->is_this(ch);
	             });

	// Копируем изменившиеся состояния из stars в tmp
	std::copy_if(stars.begin(), stars.end(), std::back_inserter(tmp),
	             [ch](const auto p_state) {
		             return p_state->is_symbol(ch);
	             });

	// Выбираем следующее состояние для всех состояний из tmp
	std::for_each(tmp.begin(), tmp.end(),
	              [](auto& p_state) {
		              ++p_state;
	              });

	// Копируем все не star состояния в buffer
	std::copy_if(tmp.begin(), tmp.end(), std::back_inserter(buffer),
	             [](const auto p_state) {
		             return ! p_state->is_star();
	             });

	// Меняем местами старый и новые буферы для не star состояний
	swap(other, buffer);

	// Копируем новые star состояния в stars
	std::copy_if(tmp.begin(), tmp.end(), std::back_inserter(stars),
	             [this](const auto p_state) {
		             return p_state->is_star() && (stars.empty() || p_state > stars.back());
	             });

	return false;
}
