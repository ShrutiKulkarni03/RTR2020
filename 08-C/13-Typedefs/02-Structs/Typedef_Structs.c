#include <stdio.h>

#define MAX_NAME_LENGTH 150

struct Employee
{
	char name[MAX_NAME_LENGTH];
	unsigned int age;
	char gender;
	double salary;
};

struct S_DATA
{
	int si;
	float sf;
	double sd;
	char sc;
};

int main(void)
{
	typedef struct Employee S_EMPLOYEE_TYPE;
	typedef struct S_DATA S_DATA_TYPE;

	struct Employee emp = { "Rucha", 22, 'F', 50000 };
	S_EMPLOYEE_TYPE emp_typedef = { "Pravin", 23, 'M', 65000 };

	struct S_DATA sd = { 30, 14.67f, 76.2375234, 'S' };
	S_DATA_TYPE sd_typedef;

	//code
	sd_typedef.si = 20;
	sd_typedef.sf = 7.096;
	sd_typedef.sd = 88.8888888;
	sd_typedef.sc = 'P';

	printf("\n\nstruct Employee : \n\n");
	printf("emp.name = %s\n", emp.name);
	printf("emp.age = %d\n", emp.age);
	printf("emp.gender = %c\n", emp.gender);
	printf("emp.salary = %lf\n", emp.salary);

	printf("\n\nS_EMPLOYEE_TYPE : \n\n");
	printf("emp_typedef.name = %s\n", emp_typedef.name);
	printf("emp_typedef.age = %d\n", emp_typedef.age);
	printf("emp_typedef.gender = %c\n", emp_typedef.gender);
	printf("emp_typedef.salary = %lf\n", emp_typedef.salary);

	printf("\n\nstruct S_DATA : \n\n");
	printf("sd.si = %d\n", sd.si);
	printf("sd.sf = %f\n", sd.sf);
	printf("sd.sd = %lf\n", sd.sd);
	printf("sd.sc = %c\n", sd.sc);

	printf("\n\nS_DATA_TYPE : \n\n");
	printf("sd_typedef.si = %d\n", sd_typedef.si);
	printf("sd_typedef.sf = %f\n", sd_typedef.sf);
	printf("sd_typedef.sd = %lf\n", sd_typedef.sd);
	printf("sd_typedef.sc = %c\n", sd_typedef.sc);

	getch();
	return(0);

}

