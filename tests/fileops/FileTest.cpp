#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "../../src/logreader/CLogReader.h"

namespace {

class FileTest : public ::testing::Test
{
protected:
	CLogReader reader;
};

TEST_F(FileTest, ShouldFailIfFileDoesntExist)
{
	char path[] = "absentfile.txt";
	EXPECT_FALSE(reader.Open(path));	
}
	
}