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

	// ������������� FSM, ���� ������� false �� �� ����� ���������
	bool start(iterator first) noexcept;

	// �������� ���������� ��� ���������� ������
	void clear() noexcept;

	// ���� true - �� �� ����� ���������� �� ������� �������
	bool check(const char ch) noexcept;

	// ���� true - �� ����� ���������� �� ����� ������� ������
	bool check_eol() const noexcept;

	// ������ �� ���������� - ����� ��� ��������, ��� ���������� ���
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
	// ���� �����-�� �� ����������� �� �����, �� �� ������ ��������
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

	// ���� ���� ���������� �������
	if (std::any_of(other.begin(), other.end(),
	                [](const auto p_state) {
		                return p_state->is_match();
	                })) {
		return true;
	}

	// �������� ������������ ��������� �� other � tmp
	std::copy_if(other.begin(), other.end(), std::back_inserter(tmp),
	             [ch](const auto p_state) {
		             return p_state->is_any() || p_state->is_this(ch);
	             });

	// �������� ������������ ��������� �� stars � tmp
	std::copy_if(stars.begin(), stars.end(), std::back_inserter(tmp),
	             [ch](const auto p_state) {
		             return p_state->is_symbol(ch);
	             });

	// �������� ��������� ��������� ��� ���� ��������� �� tmp
	std::for_each(tmp.begin(), tmp.end(),
	              [](auto& p_state) {
		              ++p_state;
	              });

	// �������� ��� �� star ��������� � buffer
	std::copy_if(tmp.begin(), tmp.end(), std::back_inserter(buffer),
	             [](const auto p_state) {
		             return ! p_state->is_star();
	             });

	// ������ ������� ������ � ����� ������ ��� �� star ���������
	swap(other, buffer);

	// �������� ����� star ��������� � stars
	std::copy_if(tmp.begin(), tmp.end(), std::back_inserter(stars),
	             [this](const auto p_state) {
		             return p_state->is_star() && (stars.empty() || p_state > stars.back());
	             });

	return false;
}
