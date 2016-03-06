#include "CLogReader.h"

#include "../regex/MyRegex.h"

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

	while (fgets(buf, bufsize, file_)) {
		auto linesize = strlen(buf);
		// Может быть что строка не поместилась в буфер
		if (linesize == bufsize - 1) {
			if (fgetc(file_) != EOF) {
				// Строка не помещается в выходной буфер
				return false;
			}
		}

		// Не отдаём движку regex символ конца строки
		if (buf[linesize - 1] == '\n') {
			--linesize;
		}

		if (p_filter_->regex_match(buf, linesize)) {
			return true;
		}
	}
	
	return false;
}