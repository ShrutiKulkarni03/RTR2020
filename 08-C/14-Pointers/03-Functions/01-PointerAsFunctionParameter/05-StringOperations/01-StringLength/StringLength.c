#include<stdio.h>
#include<stdlib.h>

#define MAX_STRING_LENGTH 256

int main(void)
{
	//function prototype
	int MyStrlen(char*);

	//variable declaration
	char* cArray = NULL;
	int iStringLength = 0;

	//code
	printf("\n\n");
	cArray = (char*)malloc(MAX_STRING_LENGTH * sizeof(char));
	if (cArray == NULL)
	{
		printf("Memory Allocation failed! Exitting Now.\n\n");
		exit(0);
	}

	//input
	printf("Enter a String : \n");
	gets_s(cArray, MAX_STRING_LENGTH);

	//output
	printf("\n\nString entered by you is : \n");
	printf("%s\n", cArray);

	//strlen
	iStringLength = MyStrlen(cArray);
	printf("Length of String is %d characters!\n\n", iStringLength);

	if (cArray)
	{
		free(cArray);
		cArray = NULL;
	}

	return(0);

}

int MyStrlen(char* str)
{
	//variable declaration
	int k;
	int string_length = 0;

	for (k = 0; k < MAX_STRING_LENGTH; k++)
	{
		if (*(str + k) == '\0')
			break;
		else
			string_length++;
	}
	return(string_length);

}
