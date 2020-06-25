#include<stdio.h>
#define MAX_STRING_LENGTH 200

int main(void)
{
	int MyStrLen(char[]);
	void MyStrCopy(char[], char[]);

	char cArray[MAX_STRING_LENGTH], cArray_SpacesRemove[MAX_STRING_LENGTH];
	int iStringLength;
	int s, k;

	printf("\n\nEnter a String\n");
	gets_s(cArray, MAX_STRING_LENGTH);

	iStringLength = MyStrLen(cArray);
	k = 0;
	for (s = 0; s < iStringLength; s++)
	{
		if (cArray[s] == ' ')
			continue;
		else
		{
			cArray_SpacesRemove[k] = cArray[s];
			k++;
		}
	}

	cArray_SpacesRemove[k] = '\0';

	printf("\n\nString Entered is\n");
	printf("%s\n", cArray);

	printf("\n\nString after removing the Spaces is\n");
	printf("%s\n", cArray_SpacesRemove);
	

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

