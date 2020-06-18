#include<stdio.h>

#define MAX_STRING_LENGTH 200

int main(void)
{

	void MyStrRev(char[], char[]);

	char cArray_Original[MAX_STRING_LENGTH];
	char cArray_Reversed[MAX_STRING_LENGTH];


	printf("\n\nEnter a String\n");
	gets_s(cArray_Original, MAX_STRING_LENGTH);

	MyStrRev(cArray_Reversed, cArray_Original);

	printf("\nThe Original String Entered is\n");
	printf("%s\n", cArray_Original);

	printf("\nThe Reversed String is\n");
	printf("%s\n", cArray_Reversed);

	return(0);

}

void MyStrRev(char str_destination[], char str_source[])
{
	int MyStrLen(char[]);

	int iStringLength = 0;
	int i, j, len;

	iStringLength = MyStrLen(str_source);

	len = iStringLength - 1;

	for (i = 0, j = len; i < iStringLength, j >= 0; i++, j--)
	{
		str_destination[i] = str_source[j];

	}
	str_destination[i] = '\0';
}

int MyStrLen(char str[])
{
	int j, string_length = 0;

	for (j = 0; j < MAX_STRING_LENGTH; j++)
	{
		if (str[j] == '\0')
			break;
		else
			string_length++;
	}
	return(string_length);

}
