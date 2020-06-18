#include<stdio.h>

#define MAX_STRING_LENGTH 200

int main(void)
{
	
	void MyStrCat(char[], char[]);

	char cArray1[MAX_STRING_LENGTH], cArray2[MAX_STRING_LENGTH];

	printf("\n\nEnter First String\n");
	gets_s(cArray1, MAX_STRING_LENGTH);

	printf("Enter Second String\n");
	gets_s(cArray2, MAX_STRING_LENGTH);

	printf("\n\nBEFORE CONCATENATION\n\n");

	printf("The Original First string entered is\n");
	printf("%s\n", cArray1);

	printf("The Original Second string entered is\n");
	printf("%s\n", cArray2);

	MyStrCat(cArray1, cArray2);

	printf("\n\nAFTER CONCATENATION\n\n");

	printf("The First string is\n");
	printf("%s\n", cArray1);

	printf("The Second string is\n");
	printf("%s\n", cArray2);

	getch();
	return(0);

}

void MyStrCat(char str_destination[], char str_source[])
{
	int MyStrLen(char[]);

	int iStringLength_Source = 0, iStringLength_Destination = 0;
	int i, j;

	iStringLength_Source = MyStrLen(str_source);
	iStringLength_Destination = MyStrLen(str_destination);

	for (i = iStringLength_Destination, j = 0; j < iStringLength_Source; i++, j++)
	{
		str_destination[i] = str_source[j];
	}
	str_destination[i] = '\0';
}

int MyStrLen(char str[])
{
	int j;
	int string_length = 0;

	for (j = 0; j < MAX_STRING_LENGTH; j++)
	{
		if (str[j] == '\0')
			break;
		else
			string_length++;

	}
	return(string_length);

}
