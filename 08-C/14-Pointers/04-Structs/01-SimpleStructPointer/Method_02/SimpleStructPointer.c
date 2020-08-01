#include <stdio.h>

struct SData
{
	int i;
	float f;
	double d;
};

int main(void)
{
	//variable declarations
	int i_size;
	int f_size;
	int d_size;
	int struct_SData_size;
	int pointer_to_struct_SData_size;
	struct SData* sData = NULL;

	//code
	
	sData = (struct SData*)malloc(sizeof(struct SData));
	if (sData == NULL)
	{
		printf("\n\nMemory Allocation to 'struct SData' failed! Exitting Now.\n\n");
		exit(0);
	}
	else
	{
		printf("\n\nMemory Allocated to 'struct SData' successful!");
	}

	//Assigning values
	(*sData).i = 3;
	(*sData).f = 15.17;
	(*sData).d = 7.15173;

	//Displaying Values	
	printf("\n\nData members of 'struct SData' are : \n\n");
	printf("i = %d\n", sData->i);
	printf("f = %f\n", sData->f);
	printf("d = %lf\n", sData->d);

	//Sizes
	i_size = sizeof(sData->i);
	f_size = sizeof(sData->f);
	d_size = sizeof(sData->d);

	//Displaying Sizes
	
	printf("\n\nSizes(bytes) of data members of 'struct SData are : \n\n");
	printf("Size of 'i' = %d bytes\n", i_size);
	printf("Size of 'f' = %d bytes\n", f_size);
	printf("Size of 'd' = %d bytes\n", d_size);

	
	struct_SData_size = sizeof(struct SData);
	pointer_to_struct_SData_size = sizeof(struct SData*);
		
	printf("\n\nSize of 'struct SData' : %d bytes\n\n", struct_SData_size);
	printf("Size of pointer to 'struct SData' : %d bytes\n\n", pointer_to_struct_SData_size);
	
	//freeing
	if (sData)
	{
		free(sData);
		sData = NULL;
		printf("Memory Allocated to 'struct SData' freed!\n\n");
	}

	getch();
	return(0);
}

