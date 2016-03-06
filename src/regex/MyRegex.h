#pragma once
#include <utility>

// Based on https://swtch.com/~rsc/regexp/regexp1.html

class MyString;
struct State;
class StateOfFSM;

class MyRegex
{
public:
	MyRegex(const char* pattern, const size_t size) noexcept;
	~MyRegex() noexcept;
	bool regex_match(const char* input, const size_t size) noexcept;

	static std::pair<char*, size_t> simplify(const char* pattern, const size_t size) noexcept;

	explicit operator bool() const noexcept;
private:
	State* p_states{nullptr};
	size_t states_size{0};

	StateOfFSM* p_sof{nullptr};
};

