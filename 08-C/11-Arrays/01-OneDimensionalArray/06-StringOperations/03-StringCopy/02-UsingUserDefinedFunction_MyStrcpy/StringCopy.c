#include<stdio.h>

#define MAX_STRING_LENGTH 200

int main(void)
{
	void MyStrCopy(char[], char[]);

	char cArray_Original[MAX_STRING_LENGTH], cArray_Copy[MAX_STRING_LENGTH];

	printf("\n\nEnter a String\n");
	gets_s(cArray_Original, MAX_STRING_LENGTH);

	MyStrCopy(cArray_Copy, cArray_Original);

	printf("\n\nThe Original String Entered is\n");
	printf("%s\n", cArray_Original);

	printf("\n\nThe Copied String is\n");
	printf("%s\n", cArray_Copy);

	return(0);

}

void MyStrCopy(char str_destination[], char str_source[])
{
	int MyStrLen(char[]);

	int iStringLength = 0;
	int i;

	iStringLength = MyStrLen(str_source);
	for (i = 0; i < iStringLength; i++)
		str_destination[i] = str_source[i];
	str_destination[i] = '\0';
}

int MyStrLen(char str[])
{
	int i;
	int string_length = 0;

	for (i = 0; i < MAX_STRING_LENGTH; i++)
	{
		if (str[i] == '\0')
			break;
		else
			string_length++;
	}
	return(string_length);

}
