#include<stdio.h>
#include<stdlib.h>

int main(void)
{
	void SAlloc(int** ptr, unsigned int number_of_elements);

	//variable declaration
	int* psArray = NULL;
	unsigned int num_elements;
	int s;

	//code
	printf("\n\nHow many elements you want in Integer Array?\n");
	scanf("%u", &num_elements);

	printf("\n\n");
	SAlloc(&psArray, num_elements);

	printf("Enter %u elements to fill up your Integer Array : \n\n", num_elements);

	for (s = 0; s < num_elements; s++)
		scanf("%d", &psArray[s]);

	printf("\n\nThe %u elements entered by you in the Integer Array : \n\n", num_elements);
	for (s = 0; s < num_elements; s++)
		printf("%u\n", psArray[s]);

	printf("\n\n");
	if (psArray)
	{
		free(psArray);
		psArray = NULL;
		printf("Memory Allocated has now been successfully freed!\n\n");
	}

	return(0);

}

void SAlloc(int** ptr, unsigned int number_of_elements)
{
	//code
	*ptr = (int*)malloc(number_of_elements * sizeof(int));
	if (*ptr == NULL)
	{
		printf("Could not Allocate Memory! Exitting Now.\n\n");
		exit(0);
	}
	printf("SAlloc() has successfully Allocated %lu Bytes for Integer Array!\n\n", (number_of_elements * sizeof(int)));
}

