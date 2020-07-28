#include<stdio.h>

int main(void)
{
	//variable declaration
	char sArray[20];
	int si;
	char* ptr_sArray = NULL;

	//code
	for (si = 0; si < 20; si++)
		sArray[si] = (char)si + 83;

	ptr_sArray = sArray;  //i.e. &sArray[si];

	printf("\n\nElements of Character Array are : \n\n");
	for (si = 0; si < 20; si++)
		printf("sArray[%d] = %c\n", si, *(ptr_sArray + si));
	printf("\n");

	printf("Elements of the Character Array are : \n\n");
	for (si = 0; si < 20; si++)
		printf("sArray[%d] = %c \t Address = %p\n", si, *(ptr_sArray + si), (ptr_sArray + si));

	getch();
	return(0);

}
