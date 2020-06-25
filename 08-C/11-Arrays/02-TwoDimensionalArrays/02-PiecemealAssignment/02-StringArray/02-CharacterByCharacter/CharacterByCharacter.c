#include<stdio.h>
#define MAX_STRING_LENGTH 200

int main(void)
{
	
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


	sArray[0][0] = 'M';
	sArray[0][1] = 'y';
	sArray[0][2] = '\0';
	sArray[1][0] = 'N';
	sArray[1][1] = 'a';
	sArray[1][2] = 'm';
	sArray[1][3] = 'e';
	sArray[1][4] = '\0';
	sArray[2][0] = 'I';
	sArray[2][1] = 's';
	sArray[2][2] = '\0';
	sArray[3][0] = 'S';
	sArray[3][1] = 'h';
	sArray[3][2] = 'r';
	sArray[3][3] = 'u';
	sArray[3][4] = 't';
	sArray[3][5] = 'i';
	sArray[3][6] = '\0';
	sArray[4][0] = 'K';
	sArray[4][1] = 'u';
	sArray[4][2] = 'l';
	sArray[4][3] = 'k';
	sArray[4][4] = 'a';
	sArray[4][5] = 'r';
	sArray[4][6] = 'n';
	sArray[4][7] = 'i';
	sArray[4][8] = '\0';

	printf("\nStrings in 2D Array are\n\n");

	for (a = 0; a < sArray_num_rows; a++)
	{
		printf("%s\n", sArray[a]);
	}
	
	getch();
	return(0);

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
