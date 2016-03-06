#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../../src/logreader/CLogReader.h"

namespace {

class TmpFile
{
public:
	TmpFile() : path_{ tmpnam(nullptr) }
	{
		FILE * file;
		fopen_s(&file, path_.c_str(), "w");
		fclose(file);
	}

	TmpFile(std::string const& content) : path_{ tmpnam(nullptr) }
	{
		FILE * file;
		fopen_s(&file, path_.c_str(), "w");

		if (file) {
			fputs(content.c_str(), file);
		}

		fclose(file);
	}

	~TmpFile()
	{
		remove(path_.c_str());
	}

	const char * path() const
	{
		return path_.c_str();
	}
private:
	std::string path_;
};

class LogReaderTest : public ::testing::Test
{
protected:
	CLogReader reader;
	char buffer[4096];
};

TEST_F(LogReaderTest, ShouldFailOnEmptyFile)
{
	TmpFile empty_file;
	ASSERT_TRUE(reader.SetFilter("?", strlen("?")));
	ASSERT_TRUE(reader.Open(empty_file.path()));
	EXPECT_FALSE(reader.GetNextLine(buffer, sizeof(buffer)));
	reader.Close();
}

TEST_F(LogReaderTest, ShouldMatchIfFileIsWholeString)
{
	TmpFile test_file{R"(a)"};
	ASSERT_TRUE(reader.SetFilter("a", strlen("a")));
	ASSERT_TRUE(reader.Open(test_file.path()));
	EXPECT_TRUE(reader.GetNextLine(buffer, sizeof(buffer)));
	EXPECT_THAT(buffer, ::testing::StrEq("a"));
	EXPECT_FALSE(reader.GetNextLine(buffer, sizeof(buffer)));
	reader.Close();
}

TEST_F(LogReaderTest, ShouldMatchIfFileIsLineAndEndOfLine)
{
	TmpFile test_file{ 
R"(a
)"
};
	ASSERT_TRUE(reader.SetFilter("a", strlen("a")));
	ASSERT_TRUE(reader.Open(test_file.path()));
	EXPECT_TRUE(reader.GetNextLine(buffer, sizeof(buffer)));
	EXPECT_THAT(buffer, ::testing::StartsWith("a"));
	EXPECT_FALSE(reader.GetNextLine(buffer, sizeof(buffer)));
	reader.Close();
}

TEST_F(LogReaderTest, ShouldMatchAllLinesFromFile)
{
	TmpFile test_file{
		R"(abc
abc
abc)"
	};
	ASSERT_TRUE(reader.SetFilter("abc", strlen("abc")));
	ASSERT_TRUE(reader.Open(test_file.path()));

	EXPECT_TRUE(reader.GetNextLine(buffer, sizeof(buffer)));
	EXPECT_THAT(buffer, ::testing::StartsWith("abc"));

	EXPECT_TRUE(reader.GetNextLine(buffer, sizeof(buffer)));
	EXPECT_THAT(buffer, ::testing::StartsWith("abc"));

	EXPECT_TRUE(reader.GetNextLine(buffer, sizeof(buffer)));
	EXPECT_THAT(buffer, ::testing::StartsWith("abc"));

	EXPECT_FALSE(reader.GetNextLine(buffer, sizeof(buffer)));
	reader.Close();
}

TEST_F(LogReaderTest, ShouldMatchSomeLinesFromFile)
{
	TmpFile test_file{
		R"(abc
cba
abc)"
	};
	ASSERT_TRUE(reader.SetFilter("abc", strlen("abc")));
	ASSERT_TRUE(reader.Open(test_file.path()));

	EXPECT_TRUE(reader.GetNextLine(buffer, sizeof(buffer)));
	EXPECT_THAT(buffer, ::testing::StartsWith("abc"));

	EXPECT_TRUE(reader.GetNextLine(buffer, sizeof(buffer)));
	EXPECT_THAT(buffer, ::testing::StartsWith("abc"));

	EXPECT_FALSE(reader.GetNextLine(buffer, sizeof(buffer)));
	reader.Close();
}

TEST_F(LogReaderTest, ShouldFailIfbufferTooShort)
{
	TmpFile test_file{ R"(aaaaaaaaaa)" };
	char short_buffer[4];

	ASSERT_TRUE(reader.SetFilter("a*", strlen("a*")));
	ASSERT_TRUE(reader.Open(test_file.path()));

	EXPECT_FALSE(reader.GetNextLine(short_buffer, sizeof(short_buffer)));
	reader.Close();
}

}
