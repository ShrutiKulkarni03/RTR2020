#include<stdio.h>

int main(void)
{
	//variable declaration
	float sArray[10];
	int si;

	//code
	for (si = 0; si < 10; si++)
		sArray[si] = (float)(si + 1) * 1.7f;

	printf("\n\nElements of Float Array are : \n\n");
	for (si = 0; si < 10; si++)
		printf("sArray[%d] = %f\n", si, sArray[si]);
	printf("\n");

	printf("Elements of the Float Array are : \n\n");
	for (si = 0; si < 10; si++)
		printf("sArray[%d] = %f \t Address = %p\n", si, sArray[si], &sArray[si]);

	getch();
	return(0);

}
