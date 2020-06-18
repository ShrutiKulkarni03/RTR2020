#include<stdio.h>
#include<string.h>

#define MAX_STRING_LENGTH 200

int main(void)
{
	char cArray_Original[MAX_STRING_LENGTH];

	printf("\n\nEnter a String\n");
	gets_s(cArray_Original, MAX_STRING_LENGTH);

	printf("\nThe Original String Entered is\n");
	printf("%s\n", cArray_Original);

	printf("\nThe Reversed String is\n");
	printf("%s\n", strrev(cArray_Original));

	getch();
	return(0);

}
