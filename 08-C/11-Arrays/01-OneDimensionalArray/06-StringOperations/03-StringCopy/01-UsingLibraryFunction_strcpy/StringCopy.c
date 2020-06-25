#include<stdio.h>

#define MAX_STRING_LENGTH 200

int main(void)
{
	

	char cArray_Original[MAX_STRING_LENGTH], cArray_Copy[MAX_STRING_LENGTH];

	printf("\n\nEnter a String\n");
	gets_s(cArray_Original, MAX_STRING_LENGTH);

	strcpy(cArray_Copy, cArray_Original);

	printf("\n\nThe Original String Entered is\n");
	printf("%s\n", cArray_Original);

	printf("\n\nThe Copied String is\n");
	printf("%s\n", cArray_Copy);

	return(0);

}

