#include<stdio.h>
#define MAX_STRING_LENGTH 200

int main(void)
{
	void MyStrCopy(char[], char[]);
	
	char sArray[5][10];

	int char_size, sArraySize, sArray_num_elements, sArray_num_rows, sArray_num_columns;
	int a;


	char_size = sizeof(char);
	sArraySize = sizeof(sArray);

	printf("\n\nSize of 2D Integer Array is %d\n", sArraySize);

	sArray_num_rows = sArraySize / sizeof(sArray[0]);
	printf("Number of rows in 2D String Array are %d\n", sArray_num_rows);

	sArray_num_columns = sizeof(sArray[0]) / char_size;
	printf("Number of columns in 2D String Array are %d\n", sArray_num_columns);

	sArray_num_elements = sArray_num_rows * sArray_num_columns;
	printf("Number of elements in 2D String Array are %d\n", sArray_num_elements);

	MyStrCopy(sArray[0], "My");
	MyStrCopy(sArray[1], "Name");
	MyStrCopy(sArray[2], "Is");
	MyStrCopy(sArray[3], "Shruti");
	MyStrCopy(sArray[4], "Kulkarni");

	printf("\nStrings in 2D Array are\n");

	for (a = 0; a < sArray_num_rows; a++)
	{
		printf("%s\n", sArray[a]);
	}
	
	getch();
	return(0);

}

void MyStrCopy(char str_destination[], char str_source[])
{
	int MyStrLen(char[]);

	int b, iString_length = 0;

	iString_length = MyStrLen(str_source);

	for (b = 0; b < iString_length; b++)
		str_destination[b] = str_source[b];

	str_destination[b] = '\0';

}

int MyStrLen(char str[])
{
	int b, String_Length = 0;

	for (b = 0; b < MAX_STRING_LENGTH; b++)
	{
		if (str[b] == '\0')
			break;
		String_Length++;

	}
	return(String_Length);

}
