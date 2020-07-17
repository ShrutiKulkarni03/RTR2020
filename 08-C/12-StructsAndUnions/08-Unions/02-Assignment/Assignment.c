#include<stdio.h>

union U_Union
{
	int si;
	float sf;
	double sd;
	char sc;
};

int main(void)
{
	//variable declaration
	union U_Union u1, u2;

	//code
	printf("\n\nMembers of Union u1 are : \n\n");

	//u1
	u1.si = 3;
	u1.sf = 1.5f;
	u1.sd = 16.19897;
	u1.sc = 'S';

	printf("u1.i = %d\n\n", u1.si);
	printf("u1.f = %f\n\n", u1.sf);
	printf("u1.d = %lf\n\n", u1.sd);
	printf("u1.c = %c\n\n", u1.sc);

	printf("Addresses of Members of Union u1 are : \n\n");
	printf("u1.i = %p\n\n", &u1.si);
	printf("u1.f = %p\n\n", &u1.sf);
	printf("u1.d = %p\n\n", &u1.sd);
	printf("u1.c = %p\n\n", &u1.sc);

	printf("U_Union u1 = %p\n\n", &u1);

	//u2
	printf("\n\nMember of Union u2 are : \n\n");

	u2.si = 3;
	printf("u2.i is %d\n\n", u2.si);

	u2.sf = 1.5f;
	printf("u2.f is %f\n\n", u2.sf);

	u2.sd = 16.19897;
	printf("u2.d is %lf\n\n", u2.sd);

	u2.sc = 'S';
	printf("u2.c is %c\n\n", u2.sc);

	printf("Addresses of Members of Union u2 are : \n\n");
	printf("u2.i = %p\n\n", &u2.si);
	printf("u2.f = %p\n\n", &u2.sf);
	printf("u2.d = %p\n\n", &u2.sd);
	printf("u2.c = %p\n\n", &u2.sc);

	printf("U_Union u2 = %p\n\n", &u2);

	getch();
	return(0);

}
