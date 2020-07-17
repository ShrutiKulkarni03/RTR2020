#include<stdio.h>

struct S_Struct
{
	int si;
	float sf;
	double sd;
	char sc;
};

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
	struct S_Struct s;
	union U_Union u;

	//code
	//struct
	printf("\n\nMembers of Struct are : \n\n");

	s.si = 3;
	s.sf = 1.5f;
	s.sd = 16.19897;
	s.sc = 'S';

	printf("s.i = %d\n\n", s.si);
	printf("s.f = %f\n\n", s.sf);
	printf("s.d = %lf\n\n", s.sd);
	printf("s.c = %c\n\n", s.sc);

	printf("Addresses of Members of Struct are : \n\n");
	printf("s.i = %p\n\n", &s.si);
	printf("s.f = %p\n\n", &s.sf);
	printf("s.d = %p\n\n", &s.sd);
	printf("s.c = %p\n\n", &s.sc);

	printf("S_Struct s = %p\n\n", &s);

	//union
	printf("\n\nMembers of Union are : \n\n");

	u.si = 3;
	printf("u.i is %d\n\n", u.si);

	u.sf = 1.5f;
	printf("u.f is %f\n\n", u.sf);

	u.sd = 16.19897;
	printf("u.d is %lf\n\n", u.sd);

	u.sc = 'S';
	printf("u.c is %c\n\n", u.sc);

	printf("Addresses of Members of Union are : \n\n");
	printf("u.i = %p\n\n", &u.si);
	printf("u.f = %p\n\n", &u.sf);
	printf("u.d = %p\n\n", &u.sd);
	printf("u.c = %p\n\n", &u.sc);

	printf("U_Union u = %p\n\n", &u);

	getch();
	return(0);

}
