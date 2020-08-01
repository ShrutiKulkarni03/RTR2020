#include<stdio.h>
#include<stdlib.h>

#define MAX_STRING_LENGTH 256

int main(void)
{
	//function prototype
	void MyStrrev(char*, char*);
	int MyStrlen(char*);

	//variable declaration
	char* cArray_original = NULL;
	char* cArray_reversed = NULL;

	int original_string_length;
	

	//code
	printf("\n\n");
	cArray_original = (char*)malloc(MAX_STRING_LENGTH * sizeof(char));
	if (cArray_original == NULL)
	{
		printf("Memory Allocation failed! Exitting Now.\n\n");
		exit(0);
	}

	//input
	printf("Enter a String : \n");
	gets_s(cArray_original, MAX_STRING_LENGTH);

	//reverse
	original_string_length = MyStrlen(cArray_original);
	cArray_reversed = (char*)malloc(original_string_length * sizeof(char));

	if (cArray_reversed == NULL)
	{
		printf("Memory Allocation for copied string failed! Exitting Now.\n\n");
		exit(0);
	}

	//copy
	MyStrrev(cArray_reversed, cArray_original);

	//output

	printf("\n\nOriginal String entered by you is : \n");
	printf("%s\n", cArray_original);

	printf("The Reversed String is : \n");
	printf("%s\n", cArray_reversed);

	if (cArray_reversed)
	{
		free(cArray_reversed);
		cArray_reversed = NULL;
		printf("\n\nMemory Allocated for Reversed string has been successfully freed!\n");
	}

	if (cArray_original)
	{
		free(cArray_original);
		cArray_original = NULL;
		printf("\n\nMemory Allocated for original string has been successfully freed!\n\n");
	}

	return(0);

}

void MyStrrev(char* str_destination, char* str_source)
{
	//function prototype
	int MyStrlen(char*);

	//variable declaration
	int iStringLength = 0;
	int s, k, len;

	//strlen
	iStringLength = MyStrlen(str_source);

	len = iStringLength - 1;

	for (s = 0, k = len; s < iStringLength, k >= 0; s++, k--)
		*(str_destination + s) = *(str_source + k);

	*(str_destination + s) = '\0';
	
}

int MyStrlen(char* str)
{
	//variable declaration
	int k;
	int string_length = 0;

	for (k = 0; k < MAX_STRING_LENGTH; k++)
	{
		if (str[k] == '\0')
			break;
		else
			string_length++;
	}
	return(string_length);

}
