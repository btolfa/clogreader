#pragma once

#include <stdio.h>

class MyRegex;

class CLogReader {
public:
	CLogReader();
	~CLogReader();

	// открытие файла, false - ошибка
	bool Open(const char* filename);

	// закрытие файла
	void Close();

	// установка фильтра строк, false - ошибка
	bool SetFilter(const char* filter, const size_t size);

	// запрос очередной найденной строки,
	// buf - буфер, bufsize - максимальная длина
	// false - конец файла или ошибка
	bool GetNextLine(char* buf, const size_t bufsize);

private:
	FILE * file_ {nullptr};
	MyRegex * p_filter_{ nullptr };	
};
