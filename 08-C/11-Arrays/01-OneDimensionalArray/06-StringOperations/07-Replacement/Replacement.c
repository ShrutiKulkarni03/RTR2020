#include<stdio.h>
#define MAX_STRING_LENGTH 200

int main(void)
{
	int MyStrLen(char[]);
	void MyStrCopy(char[], char[]);

	char cArray_Original[MAX_STRING_LENGTH], cArray_VowelsReplaced[MAX_STRING_LENGTH];
	int iStringLength;
	int s;

	printf("\n\nEnter a String\n");
	gets_s(cArray_Original, MAX_STRING_LENGTH);

	MyStrCopy(cArray_VowelsReplaced, cArray_Original);
	iStringLength = MyStrLen(cArray_VowelsReplaced);
	
	for (s = 0; s < iStringLength; s++)
	{
		switch (cArray_VowelsReplaced[s])
		{
		case 'A' :
		case 'a' :
		case 'E' :
		case 'e' :
		case 'I' :
		case 'i' :
		case 'O' :
		case 'o' :
		case 'U' :
		case 'u' :
			cArray_VowelsReplaced[s] = '_';
			break;
 		default:
			break;
		}
	}

	printf("\n\nString Entered is\n");
	printf("%s\n", cArray_Original);

	printf("\n\nString after Replecement of Vowels is\n");
	printf("%s\n", cArray_VowelsReplaced);


	return(0);

}

int MyStrLen(char str[])
{
	int s, string_length = 0;

	for (s = 0; s < MAX_STRING_LENGTH; s++)
	{
		if (str[s] == '\0')
			break;
		else
			string_length++;
	}

	return(string_length);
}

void MyStrCopy(char str_destination[], char str_source[])
{
	int MyStrLen(char[]);

	int iString_Length = 0, s;

	int iStringLength = MyStrLen(str_source);
	for (s = 0; s < iStringLength; s++)
		str_destination[s] = str_source[s];
	str_destination[s] = '\0';

}
