#include "CLogReader.h"

#include "../regex/MyRegex.h"
#include <iostream>

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
	delete p_filter_;
	p_filter_ = new (std::nothrow) MyRegex(filter, size);
	
	return p_filter_ && *p_filter_;
}

bool CLogReader::GetNextLine(char* buf, const size_t bufsize)
{
	if (!(file_ && p_filter_ && *p_filter_))
	{
		return false;
	}

	size_t linesize{ 0 };
	if (fgets(buf, bufsize, file_)) {
		linesize = strlen(buf);
		std::cout << linesize << "\n";
		return true;	
	} else {
		// Если это какая-то другая ошибка выходим
		return false;
	};

	return false;
}