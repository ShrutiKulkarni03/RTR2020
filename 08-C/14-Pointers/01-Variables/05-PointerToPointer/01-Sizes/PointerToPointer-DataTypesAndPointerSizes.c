#include<stdio.h>

struct Employee
{
	char name[100];
	int age;
	float salary;
	char sex;
	char marital_status;
};

int main(void)
{

	//code
	printf("\n\nSizes of data types and pointers to those respective data types are :\n\n");
	printf("sizeof(int) : %d \t\tSize of pointer to int (int*) : %d\t\tSize of pointer to pointer to int (int**) : %d\n", sizeof(int), sizeof(int*), sizeof(int**));
	printf("sizeof(float) : %d \t\tSize of pointer to float (float*) : %d\t\tSize of pointer to pointer to int (float**) : %d\n", sizeof(float), sizeof(float*), sizeof(float**));
	printf("sizeof(double) : %d \t\tSize of pointer to double (double*) : %d\t\tSize of pointer to pointer to double (double**) : %d\n", sizeof(double), sizeof(double*), sizeof(double**));
	printf("sizeof(char) : %d \t\tSize of pointer to char (char*) : % d\t\tSize of pointer to pointer to char (char**) : %d\n", sizeof(char), sizeof(char*), sizeof(char**));
	printf("\n\nsizeof(struct Employee) : %d \nSize of pointer to struct Employee(struct Employee*) : %d\nSize of pointer to pointer to struct Employee (struct Employee**) : %d\n\n", sizeof(struct Employee), sizeof(struct Employee*), sizeof(struct Employee**));

	getch();
	return(0);

}
