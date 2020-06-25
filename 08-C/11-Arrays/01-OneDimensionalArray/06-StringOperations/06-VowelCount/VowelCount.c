#include<stdio.h>
#define MAX_STRING_LENGTH 200

int main(void)
{
	int MyStrLen(char[]);

	char cArray[MAX_STRING_LENGTH];
	int iStringLength;
	int count_A = 0, count_E = 0, count_I = 0, count_O = 0, count_U = 0;
	int s;

	printf("\n\nEnter a String\n");
	gets_s(cArray, MAX_STRING_LENGTH);

	printf("\n\nString Entered is\n");
	printf("%s\n", cArray);

	iStringLength = MyStrLen(cArray);
	for (s = 0; s < iStringLength; s++)
	{
		switch (cArray[s])
		{
		case 'A' :
		case 'a' :
			count_A++;
			break;
		case 'E' :
		case 'e' :
			count_E++;
			break;
		case 'I' :
		case 'i' :
			count_I++;
			break;
		case 'O' :
		case 'o' :
			count_O++;
			break;
		case 'U' :
		case 'u' :
			count_U++;
			break;
		default:
			break;

		}
	}

	printf("\n\nThe Vowels and the Number of their Occurences are\n\n");

	printf("'A' has occured %d times\n", count_A);
	printf("'E' has occured %d times\n", count_E);
	printf("'I' has occured %d times\n", count_I);
	printf("'O' has occured %d times\n", count_O);
	printf("'U' has occured %d times\n", count_U);

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
