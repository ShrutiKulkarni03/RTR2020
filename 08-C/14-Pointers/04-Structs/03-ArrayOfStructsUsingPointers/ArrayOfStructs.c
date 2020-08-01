#include <stdio.h>
#include <ctype.h>

#define NAME_LENGTH 250
#define MARITAL_STATUS 125

struct Employee
{
	char name[NAME_LENGTH];
	int age;
	char sex;
	float salary;
	char marital_status;
};

int main(void)
{
	//function declarations
	void MyGetString(char[], int);

	//variable delarations
	struct Employee* sEmployeeRecord = NULL;
	int num_employees, s;

	//code
	printf("\n\nEnter Number Of Employees Whose Details You Want To Record : ");
	scanf("%d", &num_employees);

	sEmployeeRecord = (struct Employee*)malloc(sizeof(struct Employee) * num_employees);
	
	if (sEmployeeRecord == NULL)
	{
		printf("\n\nMemory Allocation for %d Employees failed! Exitting Now.\n\n", num_employees);
		exit(0);
	}
	else
		printf("\n\nMemory allocation for %d Employees successfull!", num_employees);

	//input
	for (s = 0; s < num_employees; s++)
	{
		
		printf("\n\n\nData Entry For Employee Number %d\n\n", (s + 1));

		printf("Enter Employee Name : ");
		MyGetString(sEmployeeRecord[s].name, NAME_LENGTH);
	
		printf("\nEnter Employee's Age (years) : ");
		scanf("%d", &sEmployeeRecord[s].age);
		
		printf("Enter Employee's Sex (M/m For Male, F/f For Female) : ");
		sEmployeeRecord[s].sex = getch();
		printf("%c", sEmployeeRecord[s].sex);
		sEmployeeRecord[s].sex = toupper(sEmployeeRecord[s].sex);
		
		printf("\nEnter Employee's Salary (INR) : ");
		scanf("%f", &sEmployeeRecord[s].salary);
		
		printf("Is The Employee Married? (Y/y For Yes, N/n For No) : ");
		sEmployeeRecord[s].marital_status = getch();
		printf("%c", sEmployeeRecord[s].marital_status);
		sEmployeeRecord[s].marital_status = toupper(sEmployeeRecord[s].marital_status);
	}

	//display
	
	printf("\n\nDisplaying Employee Records\n\n");
	for (s = 0; s < num_employees; s++)
	{
		printf("Employee Number %d\n\n", (s + 1));
		printf("Name : %s\n", sEmployeeRecord[s].name);
		printf("Age : %d years\n", sEmployeeRecord[s].age);

		if (sEmployeeRecord[s].sex == 'M')
			printf("Sex : Male\n");

		else if (sEmployeeRecord[s].sex == 'F')
			printf("Sex : Female\n");

		else
			printf("Sex : Invalid Data Entered\n");

		printf("Salary : Rs. %f\n", sEmployeeRecord[s].salary);

		if (sEmployeeRecord[s].marital_status == 'Y')
			printf("Marital Status : Married\n");

		else if (sEmployeeRecord[s].marital_status == 'N')
			printf("Marital Status : Unmarried\n");

		else
			printf("Marital Status : Invalid Data Entered\n");

	}

	//freeing
	if (sEmployeeRecord)
	{
		free(sEmployeeRecord);
		sEmployeeRecord = NULL;
		printf("\n\nMemory Allocated to %d Employyes freed!\n\n", num_employees);
	}

	getch();
	return(0);
}

void MyGetString(char str[], int str_size)
{	
	int s;
	char c = '\0';

	//code
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

		