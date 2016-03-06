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

private:
	dynamic_array<iterator> other;
	dynamic_array<iterator> buffer;

	dynamic_array<iterator> stars;

	dynamic_array<iterator> new_stars;
};

inline StateOfFSM::StateOfFSM(size_t max_size) noexcept : other{ max_size }, buffer{ max_size }, stars{ max_size }, new_stars{ max_size * 2 } {}

inline void StateOfFSM::clear() noexcept
{
	other.clear();
	buffer.clear();
	stars.clear();
	new_stars.clear();
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
	new_stars.clear();
	buffer.clear();

	// ��������� �� �� * ����������
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
			}
			else {
				buffer.push_back(*it);
			}
		}
	}

	// ������ ��������� �� * ����������
	for (auto it = stars.begin(); it != stars.end(); ++it) {
		if (ch == (*it)->symbol) {
			// next
			auto next = *it + 1;
			if (next->is_star()) {
				new_stars.push_back(next);
			}
			else {
				buffer.push_back(next);
			}
		}
	}

	// ������ ������� ������ � ����� ������ ��� �� * ���������
	swap(other, buffer);

	// ����� ��������� * �������� �� � stars ������ ���� ��� ������ ������
	std::copy_if(new_stars.begin(), new_stars.end(), std::back_inserter(stars), [this](const auto it)
	{
		return stars.empty() || it > stars.back();
	});

	return false;
}
