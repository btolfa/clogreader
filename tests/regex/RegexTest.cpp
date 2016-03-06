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
	auto simplified = MyRegex::simplify(str.c_str(), str.size());
	std::string result(simplified.first, simplified.second);

	EXPECT_THAT(result, StrEq(GetParam().second));
	delete[] simplified.first;
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
		{ "*****Some?" , "*Some?" },
		{ "*****?**ab*?*c" , "?*ab?*c" },
		{ "**?***????**ab*?*c??", "?????*ab?*c??" }
	};
}

INSTANTIATE_TEST_CASE_P(SimplifyRegexTest,
	SimplifyRegexTest,
	::testing::ValuesIn(simplify_cases()));

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
