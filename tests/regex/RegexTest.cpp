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
	MyRegex fsm{ "*", strlen("*") };
	EXPECT_TRUE(fsm.regex_match("", strlen("")));
	EXPECT_TRUE(fsm.regex_match("a", strlen("a")));
	EXPECT_TRUE(fsm.regex_match("abc", strlen("abc")));
}

TEST(RegexTest, ShouldMatchToSymbolAndAny)
{
	MyRegex fsm{ "a*", strlen("a*") };
	EXPECT_TRUE(fsm.regex_match("a", strlen("a")));
	EXPECT_TRUE(fsm.regex_match("abc", strlen("abc")));
	EXPECT_FALSE(fsm.regex_match("bca", strlen("bca")));
}

TEST(RegexTest, ShouldMatchToAny)
{
	MyRegex fsm{ "?", strlen("?") };
	EXPECT_TRUE(fsm.regex_match("a", strlen("a")));
	EXPECT_TRUE(fsm.regex_match("b", strlen("b")));
	EXPECT_FALSE(fsm.regex_match("", strlen("")));
	EXPECT_FALSE(fsm.regex_match("ab", strlen("ab")));
}

TEST(RegexTest, ShouldMatchAnyStar)
{
	MyRegex fsm{ "?*", strlen("?*") };
	EXPECT_TRUE(fsm.regex_match("a", strlen("a")));
	EXPECT_TRUE(fsm.regex_match("b", strlen("b")));
	EXPECT_TRUE(fsm.regex_match("ab", strlen("ab")));
	EXPECT_TRUE(fsm.regex_match("abc", strlen("abc")));
	EXPECT_FALSE(fsm.regex_match("", strlen("")));
}

TEST(RegexTest, ShouldMatchString)
{
	MyRegex fsm{ "abc", strlen("abc") };
	EXPECT_TRUE(fsm.regex_match("abc", strlen("abc")));
	EXPECT_FALSE(fsm.regex_match("", strlen("")));
	EXPECT_FALSE(fsm.regex_match("abcd", strlen("abcd")));
	EXPECT_FALSE(fsm.regex_match("cba", strlen("cba")));
	EXPECT_FALSE(fsm.regex_match("ab", strlen("ab")));
	EXPECT_FALSE(fsm.regex_match("a", strlen("a")));
}

TEST(RegexTest, ShouldMatchSymbolAndAny)
{
	MyRegex fsm{ "a?b", strlen("a?b") };
	EXPECT_TRUE(fsm.regex_match("abb", strlen("abb")));
	EXPECT_FALSE(fsm.regex_match("ab", strlen("ab")));
}

TEST(RegexTest, ShouldEndOfString)
{
	MyRegex fsm{ "*a", strlen("*a") };
	EXPECT_TRUE(fsm.regex_match("a", strlen("a")));
	EXPECT_TRUE(fsm.regex_match("aaaa", strlen("aaaa")));
	EXPECT_FALSE(fsm.regex_match("", strlen("")));
	EXPECT_FALSE(fsm.regex_match("ab", strlen("ab")));
}

TEST(RegexTest, ShouldMatchToMultiStar)
{
	MyRegex fsm{ "a*b*c", strlen("a*b*c") };
	EXPECT_TRUE(fsm.regex_match("abc", strlen("abc")));
	EXPECT_TRUE(fsm.regex_match("aeeebeeebc", strlen("aeeebeeebc")));
	EXPECT_FALSE(fsm.regex_match("", strlen("")));
	EXPECT_FALSE(fsm.regex_match("cebea", strlen("cebea")));
}

TEST(RegexTest, ShouldMatchToMultiStar2)
{
	MyRegex fsm{ "a?*b*c", strlen("a?*b*c") };
	EXPECT_TRUE(fsm.regex_match("aebc", strlen("aebc")));
	EXPECT_TRUE(fsm.regex_match("aeeebeeebc", strlen("aeeebeeebc")));
	EXPECT_FALSE(fsm.regex_match("", strlen("")));
	EXPECT_FALSE(fsm.regex_match("cebea", strlen("cebea")));
}

}
