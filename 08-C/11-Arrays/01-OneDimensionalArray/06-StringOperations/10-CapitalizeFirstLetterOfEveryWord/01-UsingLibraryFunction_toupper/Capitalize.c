#include<stdio.h>
#include<ctype.h>

#define MAX_STRING_LENGTH 200

int main(void)
{
	int MyStrLen(char[]);
	
	char cArray[MAX_STRING_LENGTH], cArray_CapitalizeInitial[MAX_STRING_LENGTH];
	int iStringLength;
	int s, k;

	printf("\n\nEnter a String\n");
	gets_s(cArray, MAX_STRING_LENGTH);

	iStringLength = MyStrLen(cArray);
	k = 0;
	for (s = 0; s < iStringLength; s++)
	{
		if (s == 0)
			cArray_CapitalizeInitial[k] = toupper(cArray[s]);
		else if (cArray[s] == ' ')
		{
			cArray_CapitalizeInitial[k] = cArray[s];
			cArray_CapitalizeInitial[k + 1] = toupper(cArray[s + 1]);
			k++;
			s++;
		}
		else
			cArray_CapitalizeInitial[k] = cArray[s];
		k++;
	}

	cArray_CapitalizeInitial[k] = '\0';

	printf("\n\nString Entered is\n");
	printf("%s\n", cArray);

	printf("\n\nString after Capitalizing Initial is\n");
	printf("%s\n", cArray_CapitalizeInitial);
	

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

