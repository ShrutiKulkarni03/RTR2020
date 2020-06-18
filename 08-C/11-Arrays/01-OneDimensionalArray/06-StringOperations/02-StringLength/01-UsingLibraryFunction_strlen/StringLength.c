#include<stdio.h>

#define MAX_STRING_LENGTH 50

int main(void)
{
	char cArray[MAX_STRING_LENGTH];
	int iStringLength = 0;

	printf("\n\nEnter a String\n");
	gets_s(cArray, MAX_STRING_LENGTH);

	printf("\n\nString Entered by you is\n");
	printf("%s\n\n", cArray); 

	iStringLength = strlen(cArray);
	printf("Length of a String is %d Characters\n", iStringLength);


	getch();
	return(0);

}
