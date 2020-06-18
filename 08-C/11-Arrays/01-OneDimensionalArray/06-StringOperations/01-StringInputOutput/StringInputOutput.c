#include<stdio.h>

#define MAX_STRING_LENGTH 50

int main(void)
{
	char cArray[MAX_STRING_LENGTH];

	printf("\n\nEnter a String\n");
	gets_s(cArray, MAX_STRING_LENGTH);

	printf("\n\nString Entered by you is\n%s\n", cArray);

	getch();
	return(0);

}
