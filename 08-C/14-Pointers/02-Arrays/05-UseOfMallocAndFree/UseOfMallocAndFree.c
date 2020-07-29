#include<stdio.h>
#include<stdlib.h>

int main(void)
{
	int* ptr_sArray = NULL;
	unsigned int intArrayLength = 0;
	int s;

	//code
	printf("\n\nEnter thr number of elements you want in your Integer Array : ");
	scanf("%d", &intArrayLength);

	ptr_sArray = (int*)malloc(sizeof(int) * intArrayLength);
	if (ptr_sArray == NULL)
	{
		printf("\n\nMemory Allocation for Integer Array Failed! Exitting now.\n\n");
		exit(0);
	}
	else
	{
		printf("\n\nMemory Allocation for Integer Array Succeeded!\n\n");
		printf("Memory Address from %p to %p have been allocated to Integer Array!\n\n", ptr_sArray, (ptr_sArray + (intArrayLength - 1)));

	}
	printf("\nEnter %d Elements for the Integer Array : \n\n", intArrayLength);
	for (s = 0; s < intArrayLength; s++)
	{
		scanf("%d", (ptr_sArray + 1));
	}

	printf("\n\nThe Integer Array Entered by you, Consisting of %d Elements : \n", intArrayLength);
	for (s = 0; s < intArrayLength; s++)
	{
		printf("ptr_sArray[%d] = %d \t\t at Address &ptr_sArray[%d] : %p\n", s, ptr_sArray[s], s, &ptr_sArray[s]);
	}

	printf("\n\n");
	for (s = 0; s < intArrayLength; s++)
	{
		printf("*(ptr_sArray + %d) = %d \t\t at Address (ptr_sArray + %d) : %p\n", s, *(ptr_sArray + s), s, (ptr_sArray + s));
	}

	if (ptr_sArray)
	{
		free(ptr_sArray);
		ptr_sArray = NULL;

		printf("\n\nMemory Allocated for Integer Array has been successfully freed!\n\n");

	}
	getch(0);
	return(0);

}
