#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


int main(void)
{
	FILE* fp;

	fprintf(stdout, "const char* nmea_log =");

	if ((fp = fopen("nmea_log.txt", "rt")) != NULL)
	{
		char line[512];
		while (!feof(fp))
		{
			memset(line, 0, sizeof(line));
			fgets(line, sizeof(line) - 1, fp);

			int len = strlen(line);
			for (int i = len - 1; i >= 0; i--)
			{
				if (!isspace(line[i]))
					break;
				line[i] = '\0';
			}

			if (line[0])
			{
				fprintf(stdout, "\n\t\"");
				fprintf(stdout, line);
				fprintf(stdout, "\\r\\n\"");
			}
		}

		fprintf(stdout, ";\n");
		fclose(fp);
	}

	return 0;
}
