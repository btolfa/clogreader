#include <gtest/gtest.h>
#include <gmock/gmock.h>

using ::testing::StrEq;

#include "../../src/regex/MyRegex.h"
#include "../../src/tools/MyString.h"

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

}
