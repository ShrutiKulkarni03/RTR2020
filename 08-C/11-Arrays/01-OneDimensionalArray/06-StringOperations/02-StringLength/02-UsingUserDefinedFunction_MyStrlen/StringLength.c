#include<stdio.h>

#define MAX_STRING_LENGTH 100

int main(void)
{
	int MyStrLen(char[]);

	char cArray[MAX_STRING_LENGTH];
	int iStringLength = 0;

	printf("\n\nEnter a String\n");
	gets_s(cArray, MAX_STRING_LENGTH);

	printf("\n\nString Entered by you is\n");
	printf("%s\n\n", cArray); 

	iStringLength = MyStrLen(cArray);
	printf("Length of a String is %d Characters\n", iStringLength);


	return(0);

}

int MyStrLen(char str[])
{
	int s;
	int string_length = 0;

	for (s = 0; s < MAX_STRING_LENGTH; s++)
	{
		if (str[s] == '\0')
			break;
		else
			string_length++;
	}
	return(string_length);

}
