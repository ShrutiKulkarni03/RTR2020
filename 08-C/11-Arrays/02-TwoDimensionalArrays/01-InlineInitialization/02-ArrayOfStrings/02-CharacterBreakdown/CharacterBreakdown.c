#include<stdio.h>
#define MAX_STRING_LENGTH 200

int main(void)
{
	int MyStrLen(char[]);

	char sArray[7][10] = { "My", "Name", "Is", "Shruti", "Kulkarni", "Of", "RTR2020" };
	int sArraySize, sArray_num_rows, iStringLength[7];
	int a, b;

	sArraySize = sizeof(sArray);
	sArray_num_rows = sArraySize / sizeof(sArray[0]);

	for (a = 0; a < sArray_num_rows; a++)
		iStringLength[a] = MyStrLen(sArray[a]);

	printf("\n\nThe String Array is\n");
	for (a = 0; a < sArray_num_rows; a++)
		printf("%s ", sArray[a]);

	printf("Strings in 2D Array are\n");
	for (a = 0; a < sArray_num_rows; a++)
	{
		printf("\nString Number %d - %s\n", (a + 1), sArray[a]);
		for (b = 0; b < iStringLength[a]; b++)
		{
			printf("Character %d is %c\n", (b + 1), sArray[a][b]);
		}
	}

	return(0);

}

int MyStrLen(char str[])
{
	int b, string_length = 0;

	for (b = 0; b < MAX_STRING_LENGTH; b++)
	{
		if (str[b] == '\0')
			break;
		else
			string_length++;
	}

	return(string_length);
}
