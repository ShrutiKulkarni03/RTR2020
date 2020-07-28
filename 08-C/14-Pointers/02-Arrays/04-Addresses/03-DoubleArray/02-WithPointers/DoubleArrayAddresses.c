#include<stdio.h>

int main(void)
{
	//variable declaration
	double sArray[10];
	int si;
	double* ptr_sArray = NULL;

	//code
	for (si = 0; si < 10; si++)
		sArray[si] = (float)(si + 1) * 1.521995f;

	ptr_sArray = sArray;  //i.e. &sArray[si];

	printf("\n\nElements of Float Array are : \n\n");
	for (si = 0; si < 10; si++)
		printf("sArray[%d] = %lf\n", si, *(ptr_sArray + si));
	printf("\n");

	printf("Elements of the Float Array are : \n\n");
	for (si = 0; si < 10; si++)
		printf("sArray[%d] = %lf \t Address = %p\n", si, *(ptr_sArray + si), (ptr_sArray + si));

	getch();
	return(0);

}
