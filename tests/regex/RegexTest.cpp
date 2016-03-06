#include <gtest/gtest.h>
#include <gmock/gmock.h>

using ::testing::StrEq;

#include "../../src/regex/MyRegex.h"
#include "../../src/tools/MyString.h"

#include <vector>
#include <list>

namespace {

using SimplifyTestCase = std::pair<std::string, std::string>;

class SimplifyRegexTest : public ::testing::TestWithParam<SimplifyTestCase> {
protected:
};

TEST_P(SimplifyRegexTest, ShouldSimplifyRegex)
{	
	const auto& str = GetParam().first;
	auto simplified = MyRegex::simplify({ str.data(), str.size() });
	std::string result(simplified.data(), simplified.size());

	EXPECT_THAT(result, StrEq(GetParam().second));
}

std::vector<SimplifyTestCase> simplify_cases()
{
	return {
		{ "Some" , "Some" },
		{ "Some*", "Some*" },
		{ "*Some" , "*Some" },
		{ "So*me" , "So*me" },
		{ "Some**" , "Some*" },
		{ "**Some" , "*Some" },
		{ "So**me" , "So*me" },
		{ "*****So******me***" , "*So*me*" },
		{ "Some?" , "Some?" },
		{ "Some?*" , "Some?*" },
		{ "*****Some?" , "*Some?" }
	};
}

INSTANTIATE_TEST_CASE_P(SimplifyRegexTest,
	SimplifyRegexTest,
	::testing::ValuesIn(simplify_cases()));

enum class state_type : uint8_t
{
	This,
	Any,
	Star,
	EndOfLine,
	Match
};

std::ostream& operator<<(std::ostream& os, const state_type& obj) {
	switch (obj) {
	case state_type::This:
		os << "This";
		break;
	case state_type::Any:
		os << "Any";
		break;
	case state_type::Star:
		os << "Star";
		break;
	case state_type::EndOfLine:
		os << "EndOfLine";
		break;
	case state_type::Match:
		os << "Match";
		break;
	}

	return os;
}

struct State
{
	state_type type;
	char symbol;

	State(const state_type type, char symbol = 0) : type{ type }, symbol{ symbol } {}

	bool is_star() const
	{
		return type == state_type::Star;
	}

	friend bool operator==(const State& lhs, const State& rhs)
	{
		return lhs.type == rhs.type
			&& lhs.symbol == rhs.symbol;
	}

	friend std::ostream& operator<<(std::ostream& os, const State& obj)
	{
		os << "type: " << obj.type;
		if (obj.type == state_type::This || obj.type == state_type::Star) {
			os << ", symbol: '" << obj.symbol << "'";
		}
		return os;
	}
};

struct FSM
{
	using states_array_t = std::vector<State>;

	explicit FSM(std::string const& pattern);

	static states_array_t generate_states(std::string const& pattern);

	bool regex_match(std::string const& input) const;

	states_array_t states;
};

FSM::FSM(std::string const& pattern) : states{generate_states(pattern)} {}

std::vector<State> FSM::generate_states(std::string const& pattern)
{
	std::vector<State> states;
	states.reserve(pattern.size());

	auto b_star = false;
	for (const auto symbol : pattern) {
		if (symbol == '*') {
			b_star = true;
			continue;
		}
		
		if (b_star) {
			states.emplace_back(state_type::Star, symbol);
			b_star = false;
		} else {
			if (symbol == '?') {
				states.emplace_back(state_type::Any);
			} else {
				states.emplace_back(state_type::This, symbol);
			}
		}		
	}

	if (b_star) {
		states.emplace_back(state_type::Match);
	} else {
		states.emplace_back(state_type::EndOfLine);
		states.emplace_back(state_type::Match);
	}

	return states;
}

struct StateOfFSM
{
	using iterator = FSM::states_array_t::const_iterator;

	StateOfFSM(iterator it)
	{
		if ((*it).type == state_type::Star) {
			stars.push_back(it);
		} else {
			other.push_back(it);
		}
	}

	bool check_char(const char ch);
	bool check_eol() const;
	bool is_empty() const;

	std::list<iterator> other;
	std::list<iterator> stars;
};

bool FSM::regex_match(std::string const& input) const
{	
	StateOfFSM sof{ states.cbegin() };

	// Обрабатываем по ходу данных
	for(const auto ch : input) {
		if (sof.check_char(ch)) {
			return true;
		}

		if (sof.is_empty()) {
			return false;
		}
	}

	// Обработка после завершения входной строки
	return sof.check_eol();
}

bool StateOfFSM::check_char(const char ch)
{

	std::vector<iterator> next_stars;
	next_stars.reserve(other.size() + stars.size());

	for (auto it = other.begin(); it != other.end();) {
		auto b_to_erase = false;

		switch((*it)->type) {
		case state_type::This:
			if ((*it)->symbol == ch) {
				// next				
				if ((++(*it))->is_star()) {					
					next_stars.push_back((*it));
					b_to_erase = true;
				}
			} else {
				b_to_erase = true;
			}
			break;
		case state_type::Any:
			// next			
			if ((++(*it))->is_star()) {
				next_stars.push_back((*it));
				b_to_erase = true;
			}
			break;
		case state_type::EndOfLine:
			b_to_erase = true;
			break;
		case state_type::Match:
			return true;
		default: break;
		}

		if (b_to_erase) {
			other.erase(it++);
		} else {
			++it;
		}
	}

	for (auto it = stars.begin(); it != stars.end(); ++it) {
		if ((*it)->symbol == ch) {
			// next
			auto next = (*it) + 1;
			if (next->is_star()) {
				next_stars.push_back(next);
			} else {
				other.push_back(next);
			}			
		}
	}

	std::copy_if(next_stars.begin(), next_stars.end(), std::back_inserter(stars), [this](const auto it)
	{
		return stars.empty() || it > stars.back();
	});	

	return false;
}

bool StateOfFSM::check_eol() const
{
	return std::any_of(other.begin(), other.end(), [](const auto it)
	{
		return it->type == state_type::EndOfLine || it->type == state_type::Match;
	});	
}

bool StateOfFSM::is_empty() const
{
	return other.empty() && stars.empty();
}

TEST(RegexTest, ShouldConvertPatternStringToStates)
{	
	EXPECT_THAT(FSM::generate_states("abc"), ::testing::ContainerEq(
		FSM::states_array_t{
		{ state_type::This, 'a' },
		{ state_type::This, 'b' },
		{ state_type::This, 'c' },
		{ state_type::EndOfLine },
		{ state_type::Match }}
	));

	EXPECT_THAT(FSM::generate_states("*"), ::testing::ContainerEq(
		FSM::states_array_t{
			{ state_type::Match } }
	));

	EXPECT_THAT(FSM::generate_states("*a"), ::testing::ContainerEq(
		FSM::states_array_t{
			{ state_type::Star, 'a' },			
			{ state_type::EndOfLine },
			{ state_type::Match } }
	));

	EXPECT_THAT(FSM::generate_states("a*"), ::testing::ContainerEq(
		FSM::states_array_t{
			{ state_type::This, 'a' },
			{ state_type::Match } }
	));

	EXPECT_THAT(FSM::generate_states("?a"), ::testing::ContainerEq(
		FSM::states_array_t{
			{ state_type::Any },
			{ state_type::This, 'a' },			
			{ state_type::EndOfLine },
			{ state_type::Match } }
	));
}

TEST(RegexTest, ShouldMatchToStar)
{
	MyRegex fsm{ "*", sizeof("*") };
	EXPECT_TRUE(fsm.regex_match("", sizeof("")));
	EXPECT_TRUE(fsm.regex_match("a", sizeof("a")));
	EXPECT_TRUE(fsm.regex_match("abc", sizeof("abc")));
}

TEST(RegexTest, ShouldMatchToSymbolAndAny)
{
	MyRegex fsm{ "a*", sizeof("a*") };
	EXPECT_TRUE(fsm.regex_match("a", sizeof("a")));
	EXPECT_TRUE(fsm.regex_match("abc", sizeof("abc")));
	EXPECT_FALSE(fsm.regex_match("bca", sizeof("bca")));
}

TEST(RegexTest, ShouldMatchToAny)
{
	MyRegex fsm{ "?", sizeof("?") };
	EXPECT_TRUE(fsm.regex_match("a", sizeof("a")));
	EXPECT_TRUE(fsm.regex_match("b", sizeof("b")));
	EXPECT_FALSE(fsm.regex_match("", sizeof("")));
	EXPECT_FALSE(fsm.regex_match("ab", sizeof("ab")));
}

TEST(RegexTest, ShouldMatchAnyStar)
{
	MyRegex fsm{ "?*", sizeof("?*") };
	EXPECT_TRUE(fsm.regex_match("a", sizeof("a")));
	EXPECT_TRUE(fsm.regex_match("b", sizeof("b")));
	EXPECT_TRUE(fsm.regex_match("ab", sizeof("ab")));
	EXPECT_TRUE(fsm.regex_match("abc", sizeof("abc")));
	EXPECT_FALSE(fsm.regex_match("", sizeof("")));
}

TEST(RegexTest, ShouldMatchString)
{
	MyRegex fsm{ "abc", sizeof("abc") };
	EXPECT_TRUE(fsm.regex_match("abc", sizeof("abc")));
	EXPECT_FALSE(fsm.regex_match("", sizeof("")));
	EXPECT_FALSE(fsm.regex_match("abcd", sizeof("abcd")));
	EXPECT_FALSE(fsm.regex_match("cba", sizeof("cba")));
	EXPECT_FALSE(fsm.regex_match("ab", sizeof("ab")));
	EXPECT_FALSE(fsm.regex_match("a", sizeof("a")));
}

TEST(RegexTest, ShouldMatchSymbolAndAny)
{
	MyRegex fsm{ "a?b", sizeof("a?b") };
	EXPECT_TRUE(fsm.regex_match("abb", sizeof("abb")));
	EXPECT_FALSE(fsm.regex_match("ab", sizeof("ab")));
}

TEST(RegexTest, ShouldEndOfString)
{
	MyRegex fsm{ "*a", sizeof("*a") };
	EXPECT_TRUE(fsm.regex_match("a", sizeof("a")));
	EXPECT_TRUE(fsm.regex_match("aaaa", sizeof("aaaa")));
	EXPECT_FALSE(fsm.regex_match("", sizeof("")));
	EXPECT_FALSE(fsm.regex_match("ab", sizeof("ab")));
}

TEST(RegexTest, ShouldMatchToMultiStar)
{
	MyRegex fsm{ "a*b*c", sizeof("a*b*c") };
	EXPECT_TRUE(fsm.regex_match("abc", sizeof("abc")));
	EXPECT_TRUE(fsm.regex_match("aeeebeeebc", sizeof("aeeebeeebc")));
	EXPECT_FALSE(fsm.regex_match("", sizeof("")));
	EXPECT_FALSE(fsm.regex_match("cebea", sizeof("cebea")));
}

TEST(RegexTest, ShouldMatchToMultiStar2)
{
	MyRegex fsm{ "a?*b*c", sizeof("a?*b*c") };
	EXPECT_TRUE(fsm.regex_match("aebc", sizeof("aebc")));
	EXPECT_TRUE(fsm.regex_match("aeeebeeebc", sizeof("aeeebeeebc")));
	EXPECT_FALSE(fsm.regex_match("", sizeof("")));
	EXPECT_FALSE(fsm.regex_match("cebea", sizeof("cebea")));
}

}
