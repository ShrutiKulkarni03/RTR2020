#include<stdio.h>
#include<stdlib.h>

int main(void)
{
	//function declaration
	void MultiplyArrayElementsByNum(int*, int, int);

	//variable declaration
	int* sArray = NULL;
	int num_elements;
	int s, num;

	//code
	printf("\n\nEnter how many elements you want in the Integer Array : ");
	scanf("%d", &num_elements);

	sArray = (int*)malloc(num_elements * sizeof(int));
	if (sArray == NULL)
	{
		printf("Memory Allocation to sArray failed! Exitting Now.\n");
		exit(0);
	}

	printf("\n\nEnter %d elements for the Integer Array : \n\n", num_elements);
	for (s = 0; s < num_elements; s++)
		printf("sArray[%d] = %d\n", s, sArray[s]);

	//one
	printf("\n\nArray before passing function MultiplyArrayElementsByNum() : \n");
	for (s = 0; s < num_elements; s++)
	{
		printf("sArray[%d] = %d\n", s, sArray[s]);
	}

	printf("\n\nEnter the number by which you want to multiply each array elements : ");
	scanf("%d", &num);

	MultiplyArrayElementsByNum(sArray, num_elements, num);

	printf("\n\nArray returned by function MultiplyArrayElementsByNum() : \n");
	for (s = 0; s < num_elements; s++)
	{
		printf("sArray[%d] = %d\n", s, sArray[s]);
	}

	if (sArray)
	{
		free(sArray);
		sArray = NULL;
		printf("\n\nMemory allocated to sArray freed!\n\n");
	}

	return(0);

}

void MultiplyArrayElementsByNum(int* array, int iNumElements, int x)
{
	int y;

	for (y = 0; y < iNumElements; y++)
		array[y] = array[y] * x;
}
