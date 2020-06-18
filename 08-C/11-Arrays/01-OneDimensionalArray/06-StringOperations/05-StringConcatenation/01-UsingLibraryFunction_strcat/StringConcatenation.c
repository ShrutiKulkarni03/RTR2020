#include<stdio.h>

#define MAX_STRING_LENGTH 200

int main(void)
{
	char cArray1[MAX_STRING_LENGTH], cArray2[MAX_STRING_LENGTH];

	printf("\n\nEnter First String\n");
	gets_s(cArray1, MAX_STRING_LENGTH);

	printf("Enter Second String\n");
	gets_s(cArray2, MAX_STRING_LENGTH);

	printf("\n\nBEFORE CONCATENATION\n\n");

	printf("The Original First string entered is\n");
	printf("%s\n", cArray1);

	printf("The Original Second string entered is\n");
	printf("%s\n", cArray2);

	strcat(cArray1, cArray2);

	printf("\n\nAFTER CONCATENATION\n\n");

	printf("The First string is\n");
	printf("%s\n", cArray1);

	printf("The Second string is\n");
	printf("%s\n", cArray2);

	getch();
	return(0);

}
