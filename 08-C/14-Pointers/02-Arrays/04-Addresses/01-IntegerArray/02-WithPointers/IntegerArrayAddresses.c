#include<stdio.h>

int main(void)
{
	//variable declaration
	int sArray[10];
	int si;
	int* ptr_sArray = NULL;

	//code
	for (si = 0; si < 10; si++)
		sArray[si] = (si + 1) * 7;

	ptr_sArray = sArray;  //i.e. &sArray[si];

	printf("\n\nElements of Integer Array are : \n\n");
	for (si = 0; si < 10; si++)
		printf("sArray[%d] = %d\n", si, *(ptr_sArray + si));
	printf("\n");

	printf("Elements of the Integer Array are : \n\n");
	for (si = 0; si < 10; si++)
		printf("sArray[%d] = %d \t Address = %p\n", si, *(ptr_sArray + si), (ptr_sArray + si));

	getch();
	return(0);

}
