#include<stdio.h>
#include<ctype.h>

#define MAX_STRING_LENGTH 200

#define SPACE ' '
#define FULLSTOP '.'
#define COMMA ','
#define EXCLAMATION '!'
#define QUESTION_MARK '?'

int main(void)
{
	int MyStrLen(char[]);
	char MyToUpper(char);
	
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
			cArray_CapitalizeInitial[k] = MyToUpper(cArray[s]);

		else if (cArray[s] == SPACE)
		{
			cArray_CapitalizeInitial[k] = cArray[s];
			cArray_CapitalizeInitial[k + 1] = MyToUpper(cArray[s + 1]);
			k++;
			s++;
		}
		else if ((cArray[s] == FULLSTOP || cArray[s] == COMMA || cArray[s] == EXCLAMATION || cArray[s] == QUESTION_MARK) && (cArray[s] != SPACE))
		{
			cArray_CapitalizeInitial[k] = cArray[s];
			cArray_CapitalizeInitial[k + 1] = SPACE;
			cArray_CapitalizeInitial[k + 2] = MyToUpper(cArray[s + 1]);

			k += 2;
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

char MyToUpper(char ch)
{
	int s_num, c;

	s_num = 'a' - 'A';

	if ((int)ch >= 97 && (int)ch <= 122)
	{
		c = (int)ch - s_num;
		return((char)c);
	}
	else
		return(ch);
}
