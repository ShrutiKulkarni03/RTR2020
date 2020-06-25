#include<stdio.h>
#include<ctype.h>

#define NUM_EMPLOYEES 4

#define NAME_LENGTH 250
#define MARITAL_STATUS 10

struct CS_Employee
{
	char name[NAME_LENGTH];
	int age;
	float salary;
	char sex;
	char marital_status;

};

int main(void)
{
	void My_Get_String(char[], int);
	
	struct CS_Employee Employee_Details[NUM_EMPLOYEES];

	int s;

	
	for (s = 0; s < NUM_EMPLOYEES; s++)
	{
		printf("\n\nEnter Employee Details of Employee Number %d\n\n", (s+1));  

		printf("Enter Name : ");
		My_Get_String(Employee_Details[s].name, NAME_LENGTH);

		printf("\nEnter Age (years) : ");
		scanf("%d", &Employee_Details[s].age);

		printf("Enter Sex (M/m For Male, F/f For Female) : ");
		Employee_Details[s].sex = getch();
		printf("%c", Employee_Details[s].sex);
		Employee_Details[s].sex = toupper(Employee_Details[s].sex);

		printf("\nEnter Salary (in Rs.) : ");
		scanf("%f", &Employee_Details[s].salary);

		printf("Is Employee married? (Y/y For Yes, N/n For No) : ");
		Employee_Details[s].marital_status = getch();
		printf("%c", Employee_Details[s].marital_status);
		Employee_Details[s].marital_status = toupper(Employee_Details[s].marital_status);

	}

	printf("\n\nEMPLOYEE DETAILS\n\n");

	for (s = 0; s < NUM_EMPLOYEES; s++)
	{
		printf("EMPLOYEE NUMBER %d\n\n", (s + 1));
		printf("Name : %s\n", Employee_Details[s].name);
		printf("Age : %d years\n", Employee_Details[s].age);

		if (Employee_Details[s].sex == 'M')
			printf("Sex : Male\n");

		else if (Employee_Details[s].sex == 'F')
			printf("Sex : Female\n");

		else
			printf("Sex : Invalid Input\n");

		printf("Salary : Rs. %f\n", Employee_Details[s].salary);

		if (Employee_Details[s].marital_status == 'Y')
			printf("Marital Status : Married\n");

		else if (Employee_Details[s].marital_status == 'N')
			printf("Marital Status : Unmarried\n");

		else
			printf("Marital Status : Invalid Input\n");

	}

	getch();
	return(0);

}

void My_Get_String(char str[], int str_size)
{
	int s;
	char c = '\0';

	s = 0;
	do
	{
		c = getch();
		str[s] = c;
		printf("%c", str[s]);
		s++;
	} while ((c != '\r') && (s < str_size));

	if (s == str_size)
		str[s - 1] = '\0';
	else
		str[s] = '\0';
}
