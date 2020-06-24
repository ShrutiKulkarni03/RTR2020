#include<stdio.h>

#define NAME_LENGTH 250
#define MARITAL_STATUS 10

struct CS_Employee
{
	char name[NAME_LENGTH];
	int age;
	float salary;
	char sex;
	char marital_status[MARITAL_STATUS];

};

int main(void)
{
	struct CS_Employee Employee_Details[4];

	char Employee_Aditi[] = "Aditi";
	char Employee_Pravin[] = "Pravin";
	char Employee_Rahul[] = "Rahul";
	char Employee_Jagruti[] = "Jagruti";

	int s;

	//1

	strcpy(Employee_Details[0].name, Employee_Aditi);
	Employee_Details[0].age = 25;
	Employee_Details[0].salary = 20000.0f;
	Employee_Details[0].sex = 'F';
	strcpy(Employee_Details[0].marital_status, "Unmarried");

	//2

	strcpy(Employee_Details[1].name, Employee_Pravin);
	Employee_Details[1].age = 23;
	Employee_Details[1].salary = 30000.0f;
	Employee_Details[1].sex = 'M';
	strcpy(Employee_Details[1].marital_status, "Unmarried");

	//3

	strcpy(Employee_Details[2].name, Employee_Rahul);
	Employee_Details[2].age = 34;
	Employee_Details[2].salary = 55000.0f;
	Employee_Details[2].sex = 'M';
	strcpy(Employee_Details[2].marital_status, "Married");

	//4

	strcpy(Employee_Details[3].name, Employee_Jagruti);
	Employee_Details[3].age = 30;
	Employee_Details[3].salary = 35000.0f;
	Employee_Details[3].sex = 'F';
	strcpy(Employee_Details[3].marital_status, "Married");

	printf("\n\nEmployee Details\n\n");
	for (s = 0; s < 4; s++)
	{
		printf("\nEmployee Number %d \n", (s + 1));
		printf("Name : %s \n", Employee_Details[s].name);
		printf("Age : %d years \n", Employee_Details[s].age);

		if (Employee_Details[s].sex == 'M' || Employee_Details[s].sex == 'm')
			printf("Sex : Male\n");

		else
			printf("Sex : Female\n");

		printf("Salary : Rs. %f\n", Employee_Details[s].salary);
		printf("Marital Status : %s\n", Employee_Details[s].marital_status);

	}

	getch();
	return(0);

}
