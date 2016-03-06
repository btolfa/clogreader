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

	state_type type;
	char symbol;
};