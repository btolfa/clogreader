#pragma once

class CLogReader {
public:
	CLogReader();
	~CLogReader();

	bool    Open();                       // открытие файла, false - ошибка
	void    Close();                         // закрытие файла

	bool    SetFilter(const char *filter);   // установка фильтра строк, false - ошибка
	bool    GetNextLine(char *buf,           // запрос очередной найденной строки,
		const int bufsize);  // buf - буфер, bufsize - максимальная длина
							 // false - конец файла или ошибка
};