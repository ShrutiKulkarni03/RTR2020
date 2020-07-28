#include<stdio.h>

int main(void)
{
	//variable declaration
	int sArray[10];
	int si;

	//code
	for (si = 0; si < 10; si++)
		sArray[si] = (si + 1) * 3;

	printf("\n\nElements of Integer Array are : \n\n");
	for (si = 0; si < 10; si++)
		printf("sArray[%d] = %d\n", si, sArray[si]);
	printf("\n");

	printf("Elements of the Integer Array are : \n\n");
	for (si = 0; si < 10; si++)
		printf("sArray[%d] = %d \t Address = %p\n", si, sArray[si], &sArray[si]);

	getch();
	return(0);

}
