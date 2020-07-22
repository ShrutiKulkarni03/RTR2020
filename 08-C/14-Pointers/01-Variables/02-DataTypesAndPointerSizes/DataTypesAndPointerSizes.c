#include <stdio.h>

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
	printf("sizeof(int) : %d \t\t\t Size of pointer to int (int*) : %d\n", sizeof(int), sizeof(int*));
	printf("sizeof(float) : %d \t\t\t Size of pointer to float (float*) : %d\n", sizeof(float), sizeof(float*));
	printf("sizeof(double) : %d \t\t\t Size of pointer to double (double*) : %d\n", sizeof(double), sizeof(double*));
	printf("sizeof(char) : %d \t\t\t Size of pointer to char (char*) : % d\n", sizeof(char), sizeof(char*));
	printf("sizeof(struct Employee) : %d \t\t Size of pointer to struct Employee(struct Employee*) : %d\n\n", sizeof(struct Employee), sizeof(struct Employee*));
	
	getch();
	return(0);
}
