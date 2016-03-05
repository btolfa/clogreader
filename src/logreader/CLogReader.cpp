#include "CLogReader.h"

CLogReader::CLogReader()
{
}

CLogReader::~CLogReader()
{
	Close();
}

bool CLogReader::Open(const char *filename)
{
	if (file_)
	{
		Close();
	}

	
	return fopen_s(&file_, filename, "r") == 0;
}

void CLogReader::Close()
{
	if (file_)
	{
		fclose(file_);
		file_ = nullptr;
	}
}

bool CLogReader::SetFilter(const char* filter, const size_t size)
{
	if (! file_)
	{
		return false;
	}

	filter_str_ = MyString{ filter, size };

	return bool(filter_str_);	
}

bool CLogReader::GetNextLine(char* buf, const size_t bufsize)
{
	if ( (! file_) || (! filter_str_) )
	{
		return false;
	}
	return true;
}