#pragma once

#include <cstdint>

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