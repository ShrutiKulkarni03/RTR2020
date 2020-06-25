#include<stdio.h>
#define MAX_STRING_LENGTH 200

int main(void)
{
	int MyStrLen(char[]);

	char sArray[7][10] = { "My", "Name", "Is", "Shruti", "Kulkarni", "Of", "RTR2020" };
	int char_size, sArraySize, sArray_num_elements, sArray_num_rows, sArray_num_columns, sActual_num_char = 0;
	int a;

	char_size = sizeof(char);
	sArraySize = sizeof(sArray);

	printf("\n\nSize of 2D String Array is %d\n", sArraySize);

	sArray_num_rows = sArraySize / sizeof(sArray[0]);
	printf("Number of rows in 2D String Array are %d\n", sArray_num_rows);

	sArray_num_columns = sizeof(sArray[0]) / char_size;
	printf("Number of columns in 2D String Array are %d\n", sArray_num_columns);

	sArray_num_elements = sArray_num_rows / sArray_num_columns;
	printf("Maximum number of elements in 2D String Array are %d\n", sArray_num_elements);

	for (a = 0; a < sArray_num_rows; a++)
	{
		sActual_num_char += MyStrLen(sArray[a]);
	}

	printf("Actual number of elements in 2D String Array are %d\n", sActual_num_char);

	printf("\nStrings in 2D Array are\n");

	printf("%s ", sArray[0]);
	printf("%s ", sArray[1]);
	printf("%s ", sArray[2]);
	printf("%s ", sArray[3]);
	printf("%s ", sArray[4]);
	printf("%s ", sArray[5]);
	printf("%s \n\n", sArray[6]);
	
	getch();
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