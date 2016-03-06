#pragma once

#include <cstdint>

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

	bool is_symbol(const char ch) const noexcept
	{
		return ch == symbol;
	}

	bool is_any() const noexcept {
		return type == state_type::Any;
	}

	bool is_this(const char ch) const noexcept {
		return (type == state_type::This && symbol == ch);
	}

	bool is_match() const noexcept {
		return type == state_type::Match;
	}

	state_type type;
	char symbol;
};