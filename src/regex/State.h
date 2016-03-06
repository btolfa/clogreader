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

	state_type type;
	char symbol;
};