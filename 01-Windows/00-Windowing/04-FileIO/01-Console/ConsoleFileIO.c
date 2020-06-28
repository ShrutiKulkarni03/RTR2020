#include<stdio.h>

int main(void)
{
	FILE* pFile = NULL;

	if (fopen_s(&pFile, "SPKLog.txt", "w") != 0)
	{
		printf("Cannot Open The Desired Window");
		exit(0);
	}

	fprintf(pFile, "India Is My Country.\n");

	fclose(pFile);

	pFile = NULL;

	return(0);
}

