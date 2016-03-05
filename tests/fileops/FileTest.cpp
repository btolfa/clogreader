#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <stdio.h>
#include <string>
#include "../../src/logreader/CLogReader.h"

namespace {

class TmpFile
{
public:
	explicit TmpFile() : path_{tmpnam(nullptr)}
	{		
		FILE * file;
		fopen_s(&file, path_.c_str(), "w");
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

class FileTest : public ::testing::Test
{	
protected:	
	TmpFile tmp_file;
	std::string filter{ "*" };
	char buf[10];

	CLogReader reader;

};

TEST_F(FileTest, ShouldFailIfFileAbsent)
{
	auto absentpath = tmpnam(nullptr);
	EXPECT_FALSE(reader.Open(absentpath));
}

TEST_F(FileTest, ShouldSetFilter)
{
	EXPECT_TRUE(reader.SetFilter(filter.c_str(), filter.size()));
}

TEST_F(FileTest, ShouldOpenFile)
{
	EXPECT_TRUE(reader.Open(tmp_file.path()));
}

TEST_F(FileTest, ShouldFailGetNextLineIfNotOpened)
{	
	ASSERT_TRUE(reader.SetFilter(filter.c_str(), filter.size()));
	
	EXPECT_FALSE(reader.GetNextLine(buf, sizeof(buf)));
}

TEST_F(FileTest, ShouldFailGetNextLineIfNotSetFilter)
{
	ASSERT_TRUE(reader.Open(tmp_file.path()));
	
	EXPECT_FALSE(reader.GetNextLine(buf, sizeof(buf)));
}
	
}