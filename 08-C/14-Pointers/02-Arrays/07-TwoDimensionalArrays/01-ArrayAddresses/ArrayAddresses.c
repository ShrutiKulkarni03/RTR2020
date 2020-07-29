#include<stdio.h>
#include<stdlib.h>

#define NUM_ROWS 5
#define NUM_COLUMNS 5

int main(void)
{
	//variable declaration
	int sArray[NUM_ROWS][NUM_COLUMNS];
	int s, k;

	//code
	for (s = 0; s < NUM_ROWS; s++)
	{
		for (k = 0; k < NUM_COLUMNS; k++)
			sArray[s][k] = (s + 1) * (k + 1);
	}

	printf("\n\n2D Integer Array Elements along with Addresses : \n\n");
	for (s = 0; s < NUM_ROWS; s++)
	{
		for (k = 0; k < NUM_COLUMNS; k++)
		{
			printf("sArray[%d][%d] = %d \t at Address : %p\n", s, k, sArray[s][k], &sArray[s][k]);
		}
		printf("\n");
	}

	getch();
	return(0);

}
