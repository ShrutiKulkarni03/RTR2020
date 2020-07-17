#include<stdio.h>

struct S_Struct
{
	int si;
	float sf;
	double sd;
	char sc;
};

int main(void)
{
	//function declaration
	struct S_Struct AddStructMembers(struct S_Struct, struct S_Struct, struct S_Struct);


	//variable declaration
	struct S_Struct data1, data2, data3, answer_data;

	//code
	//1
	printf("\n\nData1\n\n");

	printf("Enter Integer Value for 'i' of 'struct S_Struct data1' : ");
	scanf("%d", &data1.si);

	printf("Enter Floating-Point Value for 'f' of 'struct S_Struct data1' : ");
	scanf("%f", &data1.sf);

	printf("Enter 'Double' Value for 'd' of 'struct S_Struct data1' : ");
	scanf("%lf", &data1.sd);

	printf("Enter Character Value for 'c' of 'struct S_Struct data1' : ");
	data1.sc = getch();
	printf("%c", data1.sc);

	//2
	printf("\n\nData2\n\n");

	printf("Enter Integer Value for 'i' of 'struct S_Struct data2' : ");
	scanf("%d", &data3.si);

	printf("Enter Floating-Point Value for 'f' of 'struct S_Struct data2' : ");
	scanf("%f", &data2.sf);

	printf("Enter 'Double' Value for 'd' of 'struct S_Struct data2' : ");
	scanf("%lf", &data2.sd);

	printf("Enter Character Value for 'c' of 'struct S_Struct data2' : ");
	data2.sc = getch();
	printf("%c", data2.sc);

	//3
	printf("\n\nData3\n\n");

	printf("Enter Integer Value for 'i' of 'struct S_Struct data3' : ");
	scanf("%d", &data3.si);

	printf("Enter Floating-Point Value for 'f' of 'struct S_Struct data3' : ");
	scanf("%f", &data3.sf);

	printf("Enter 'Double' Value for 'd' of 'struct S_Struct data3' : ");
	scanf("%lf", &data3.sd);

	printf("Enter Character Value for 'c' of 'struct S_Struct data3' : ");
	data3.sc = getch();
	printf("%c", data3.sc);

	//function call
	answer_data = AddStructMembers(data1, data2, data3);

	printf("\n\nAnswer\n\n");
	printf("answer_data.i = %d\n", answer_data.si);
	printf("answer_data.f = %f\n", answer_data.sf);
	printf("answer_data.d = %lf\n", answer_data.sd);

	answer_data.sc = data1.sc;
	printf("\n\nanswer_data.c (data1) = %c", answer_data.sc);

	answer_data.sc = data2.sc;
	printf("\nanswer_data.c (data2) = %c", answer_data.sc);

	answer_data.sc = data3.sc;
	printf("\nanswer_data.c (data3) = %c\n\n", answer_data.sc);
	

	getch();
	return(0);

}

struct S_Struct AddStructMembers(struct S_Struct s1, struct S_Struct s2, struct S_Struct s3)
{
	struct S_Struct ans;

	ans.si = s1.si + s2.si + s3.si;
	ans.sf = s1.sf + s2.sf + s3.sf;
	ans.sd = s1.sd + s2.sd + s3.sd;

	return(ans);

}
