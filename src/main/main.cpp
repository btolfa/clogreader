#include "../logreader/CLogReader.h"

#include <cstring>

int main(int argc, char* argv[]) {
	if (argc < 3)
	{
		printf("Usage\n");
		printf("\tclogreader PATTERN FILENAME\n");
		printf("Example:\n");
		printf("\tclogreader \"order*closed\" test.log\n");
	} else {
		CLogReader reader;

		auto filename = argv[2];
		auto pattern = argv[1];

		if (! reader.Open(filename)) {
			printf("Error opening file: %s\n", filename);
			return 1;
		}

		if (! reader.SetFilter(pattern, strlen(pattern))) {
			printf("Error with pattern: %s\n", pattern);
		}

		char buffer[4096];
		while (reader.GetNextLine(buffer, sizeof(buffer))) {
			printf("%s", buffer);
		}

		printf("\n");
		reader.Close();
	}

	
	return 0;
}
