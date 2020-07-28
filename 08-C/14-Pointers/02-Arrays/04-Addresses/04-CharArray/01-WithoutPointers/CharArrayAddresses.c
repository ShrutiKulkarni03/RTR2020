#include<stdio.h>

int main(void)
{
	//variable declaration
	char sArray[20];
	int si;

	//code
	for (si = 0; si < 20; si++)
		sArray[si] = (float)si + 83;

	printf("\n\nElements of Character Array are : \n\n");
	for (si = 0; si < 20; si++)
		printf("sArray[%d] = %c\n", si, sArray[si]);
	printf("\n");

	printf("Elements of the Character Array are : \n\n");
	for (si = 0; si < 20; si++)
		printf("sArray[%d] = %c \t Address = %p\n", si, sArray[si], &sArray[si]);

	getch();
	return(0);

}
