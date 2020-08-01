#include<stdio.h>
#include<stdlib.h>

#define MAX_STRING_LENGTH 512

int main(void)
{
	char* ReplaceVowelWithHash(char*);

	char string[MAX_STRING_LENGTH];
	char* replaced_string = NULL;

	printf("\n\nEnter String : \n");
	gets_s(string, MAX_STRING_LENGTH);

	replaced_string = ReplaceVowelWithHash(string);

	if (replaced_string == NULL)
	{
		printf("ReplaceVowelWithHash() function has failed! Exitting now.\n");
		exit(0);
	}

	printf("\n\nReplaced String is : \n");
	printf("%s\n\n", replaced_string);

	if (replaced_string)
	{
		free(replaced_string);
		replaced_string = NULL;
	}

	return(0);

}

char* ReplaceVowelWithHash(char* s)
{
	void MyStrcpy(char*, char*);
	int MyStrlen(char*);

	char* new_string = NULL;
	int i;

	new_string = (char*)malloc(MyStrlen(s) * sizeof(char));
	if (new_string == NULL)
	{
		printf("Could not allocate memory for new string!\n");
		return(NULL);
	}

	MyStrcpy(new_string, s);
	for (i = 0; i < MyStrlen(new_string); i++)
	{
		switch (new_string[i])
		{
		case 'A':
		case 'a':

		case 'E':
		case 'e':

		case 'I':
		case 'i':

		case 'O':
		case 'o':

		case 'U':
		case 'u':

			new_string[i] = '#';
			break;
		default:
			break;

		}
	}

	return(new_string);

}

void MyStrcpy(char* str_destination, char* str_source)
{
	int MyStrlen(char*);

	int iStringLength = 0;
	int s;

	iStringLength = MyStrlen(str_source);

	for (s = 0; s < iStringLength; s++)
		*(str_destination + s) = *(str_source + s);

	*(str_destination + s) = '\0';

}

int MyStrlen(char* str)
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
