#include<stdio.h>
#define MAX_STRING_LENGTH 200

int main(void)
{
	int MyStrLen(char[]);
	void MyStrCopy(char[], char[]);

	char cArray[MAX_STRING_LENGTH];
	int iStringLength;
	int s;
	int word_count = 0, space_count = 0;

	printf("\n\nEnter a String\n");
	gets_s(cArray, MAX_STRING_LENGTH);

	iStringLength = MyStrLen(cArray);
	
	for (s = 0; s < iStringLength; s++)
	{
		switch (cArray[s])
		{
		case 32 :
			space_count++;
			break;
 		default:
			break;
		}
	}

	word_count = space_count + 1;

	printf("\n\nString Entered is\n");
	printf("%s\n", cArray);

	printf("\n\nNumber of Spaces in the String are %d\n", space_count);
	printf("\n\nNumber of Words in the String are %d\n", word_count);

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

