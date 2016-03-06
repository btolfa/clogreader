#pragma once

#include <stdio.h>

class MyRegex;

class CLogReader {
public:
	CLogReader();
	~CLogReader();

	// �������� �����, false - ������
	bool Open(const char* filename);

	// �������� �����
	void Close();

	// ��������� ������� �����, false - ������
	bool SetFilter(const char* filter, const size_t size);

	// ������ ��������� ��������� ������,
	// buf - �����, bufsize - ������������ �����
	// false - ����� ����� ��� ������
	bool GetNextLine(char* buf, const size_t bufsize);

private:
	FILE * file_ {nullptr};
	MyRegex * p_filter_{ nullptr };	
};
