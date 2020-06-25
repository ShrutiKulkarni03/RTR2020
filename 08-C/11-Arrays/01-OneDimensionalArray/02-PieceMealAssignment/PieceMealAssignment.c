#include<stdio.h>

int main(void)
{
	int sArray1[10], sArray2[10];

	sArray1[0] = 7;
	sArray1[1] = 14;
	sArray1[2] = 21;
	sArray1[3] = 28;
	sArray1[4] = 35;
	sArray1[5] = 42;
	sArray1[6] = 49;
	sArray1[7] = 56;
	sArray1[8] = 63;
	sArray1[9] = 70;

	printf("\n\nPiece-meal Initialization of Integer type Array\n\n");
	printf("1st Element of Array (0th Index) is %d\n", sArray1[0]);
	printf("2nd Element of Array (1st Index) is %d\n", sArray1[1]);
	printf("3rd Element of Array (2nd Index) is %d\n", sArray1[2]);
	printf("4th Element of Array (3rd Index) is %d\n", sArray1[3]);
	printf("5th Element of Array (4th Index) is %d\n", sArray1[4]);
	printf("6th Element of Array (5th Index) is %d\n", sArray1[5]);
	printf("7th Element of Array (6th Index) is %d\n", sArray1[6]);
	printf("8th Element of Array (7th Index) is %d\n", sArray1[7]);
	printf("9th Element of Array (8th Index) is %d\n", sArray1[8]);
	printf("10th Element of Array (9th Index) is %d\n\n", sArray1[9]);


	printf("Enter 1st Element (0th Index) of an Integer type Array :");
	scanf("%d", &sArray2[0]);

	printf("Enter 2nd Element (1st Index) of an Integer type Array :");
	scanf("%d", &sArray2[1]);

	printf("Enter 3rd Element (2nd Index) of an Integer type Array :");
	scanf("%d", &sArray2[2]);

	printf("Enter 4th Element (3rd Index) of an Integer type Array :");
	scanf("%d", &sArray2[3]);

	printf("Enter 5th Element (4th Index) of an Integer type Array :");
	scanf("%d", &sArray2[4]);

	printf("Enter 6th Element (5th Index) of an Integer type Array :");
	scanf("%d", &sArray2[5]);

	printf("Enter 7th Element (6th Index) of an Integer type Array :");
	scanf("%d", &sArray2[6]);

	printf("Enter 8th Element (7th Index) of an Integer type Array :");
	scanf("%d", &sArray2[7]);

	printf("Enter 9th Element (8th Index) of an Integer type Array :");
	scanf("%d", &sArray2[8]);

	printf("Enter 10th Element (9th Index) of an Integer type Array :");
	scanf("%d", &sArray2[9]);



	printf("\n\n1st Element (0th Index) of an Integer type Array is %d\n", sArray2[0]);
	printf("2nd Element (1st Index) of an Integer type Array is %d\n", sArray2[1]);
	printf("3rd Element (2nd Index) of an Integer type Array is %d\n", sArray2[2]);
	printf("4th Element (3rd Index) of an Integer type Array is %d\n", sArray2[3]);
	printf("5th Element (4th Index) of an Integer type Array is %d\n", sArray2[4]);
	printf("6th Element (5th Index) of an Integer type Array is %d\n", sArray2[5]);
	printf("7th Element (6th Index) of an Integer type Array is %d\n", sArray2[6]);
	printf("8th Element (7th Index) of an Integer type Array is %d\n", sArray2[7]);
	printf("9th Element (8th Index) of an Integer type Array is %d\n", sArray2[8]);
	printf("10th Element (9th Index) of an Integer type Array is %d\n", sArray2[9]);
	
	getch();
	return(0);
}

