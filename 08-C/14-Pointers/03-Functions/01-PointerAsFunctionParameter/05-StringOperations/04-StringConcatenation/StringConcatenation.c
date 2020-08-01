#include<stdio.h>

#define MAX_STRING_LENGTH 256

int main(void)
{
	//function prototype
	void MyStrcat(char*, char*);
	int MyStrlen(char*);

	//variable declaration
	char* cArray_one = NULL;
	char* cArray_two = NULL;

	//int original_string_length;
	

	//code
	printf("\n\n");
	cArray_one = (char*)malloc(MAX_STRING_LENGTH * sizeof(char));
	if (cArray_one == NULL)
	{
		printf("Memory Allocation of First String failed! Exitting Now.\n\n");
		exit(0);
	}

	//input
	printf("Enter First String : \n");
	gets_s(cArray_one, MAX_STRING_LENGTH);

	cArray_two = (char*)malloc(MAX_STRING_LENGTH * sizeof(char));
	if (cArray_two == NULL)
	{
		printf("Memory Allocation of Second String failed! Exitting Now.\n\n");
		exit(0);
	}

	printf("Enter Second String : \n");
	gets_s(cArray_two, MAX_STRING_LENGTH);

	//concat

	printf("\n\nBefore Concatenation\n\n");
	printf("The original 1st String entered is : \n");
	printf("%s\n", cArray_one);

	printf("The original 2nd String entered is : \n");
	printf("%s\n", cArray_two);
	

	MyStrcat(cArray_one, cArray_two);

	printf("\n\nAfter Concatenation\n\n");
	printf("'cArray_one[]' is : \n");
	printf("%s\n", cArray_one);

	printf("'cArray_two[]' is : \n");
	printf("%s\n", cArray_two);

	//freeing

	if (cArray_two)
	{
		free(cArray_one);
		cArray_one = NULL;
		printf("Memory Allocated to 2nd String freed!\n");

	}

	return(0);


}

void MyStrcat(char* str_destination, char* str_source)
{
	//function prototype
	int MyStrlen(char*);

	//variable declaration
	int iStringLength_source = 0, iStringLength_Destination = 0;
	int s, k;

	//strlen
	iStringLength_source = MyStrlen(str_source);
	iStringLength_Destination = MyStrlen(str_destination);


	for (s = iStringLength_Destination, k = 0; k < iStringLength_source; s++, k++)
	{
		*(str_destination + s) = *(str_source + k);
	}
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
