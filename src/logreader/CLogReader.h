#pragma once

class CLogReader {
public:
	CLogReader();
	~CLogReader();

	bool    Open();                       // �������� �����, false - ������
	void    Close();                         // �������� �����

	bool    SetFilter(const char *filter);   // ��������� ������� �����, false - ������
	bool    GetNextLine(char *buf,           // ������ ��������� ��������� ������,
		const int bufsize);  // buf - �����, bufsize - ������������ �����
							 // false - ����� ����� ��� ������
};