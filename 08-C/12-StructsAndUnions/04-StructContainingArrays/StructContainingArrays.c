#include<stdio.h>

#define INT_ARRAY_SIZE 8
#define FLOAT_ARRAY_SIZE 5
#define CHAR_ARRAY_SIZE 26

#define NUM_STRINGS 6
#define MAX_CHARACTERS_PER_STRING 20
#define ALPHABET_BEGINNING 65

struct MyData1
{
	int I_Array[INT_ARRAY_SIZE];
	float F_Array[FLOAT_ARRAY_SIZE];

};

struct MyData2
{
	char C_Array[CHAR_ARRAY_SIZE];
	char STR_Array[NUM_STRINGS][MAX_CHARACTERS_PER_STRING];

};

int main(void)
{
	struct MyData1 Data1;
	struct MyData2 Data2;
	int s;

	Data1.F_Array[0] = 1.1f;
	Data1.F_Array[1] = 1.2f;
	Data1.F_Array[2] = 1.3f;
	Data1.F_Array[3] = 1.4f;
	Data1.F_Array[4] = 1.5f;

	//Loop

	printf("\n\nEnter %d Integers\n", INT_ARRAY_SIZE);
	for (s = 0; s < INT_ARRAY_SIZE; s++)
		scanf("%d", &Data1.I_Array[s]);
	//Loop

	for (s = 0; s < CHAR_ARRAY_SIZE; s++)
		Data2.C_Array[s] = (char)(s + ALPHABET_BEGINNING);


	strcpy(Data2.STR_Array[0], "Shruti");
	strcpy(Data2.STR_Array[1], "Kulkarni");
	strcpy(Data2.STR_Array[2], "of");
	strcpy(Data2.STR_Array[3], "Batch");
	strcpy(Data2.STR_Array[4], "RTR2020");
	strcpy(Data2.STR_Array[5], "003");

	printf("\n\nMembers of struct Data1 with assigned values are : \n");

	printf("Integer Array (Data1.I_Array[]) : \n");
	for (s = 0; s < INT_ARRAY_SIZE; s++)
		printf("Data1.I_Array[%d] = %d\n", s, Data1.I_Array[s]);

	printf("\nFloating-Point Array (Data1.F_Array[]) : \n");
	for (s = 0; s < FLOAT_ARRAY_SIZE; s++)
		printf("Data1.F_Array[%d] = %f\n", s, Data1.F_Array[s]);


	printf("\n\nMembers of struct Data2 with assigned values are : \n");

	printf("\nCharacter Array (Data2.C_Array[]) : \n");
	for (s = 0; s < CHAR_ARRAY_SIZE; s++)
		printf("Data2.C_Array[%d] = %c\n", s, Data2.C_Array[s]);

	printf("\nString Array (Data2.STR_Array[]) : \n");
	for (s = 0; s < NUM_STRINGS; s++)
		printf("%s ", Data2.STR_Array[s]);

	printf("\n\n");


	getch();
	return(0);

}
