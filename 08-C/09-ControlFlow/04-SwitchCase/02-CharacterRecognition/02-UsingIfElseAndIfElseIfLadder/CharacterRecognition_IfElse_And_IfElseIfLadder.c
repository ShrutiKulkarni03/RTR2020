#include<stdio.h>
#include<conio.h>

#define UPPER_B 65
#define UPPER_E 90

#define LOWER_B 97
#define LOWER_E 122

#define DIGITS_B 48
#define DIGITS_E 57

int main(void)
{
	char sk_c;
	int sk_c_value;

	printf("\n\n");
	printf("Enter a Character\n");
	sk_c = getch();

	printf("\n");

	if (sk_c == 'A' || sk_c == 'a' || sk_c == 'E' || sk_c == 'e' || sk_c == 'I' || sk_c == 'i' || sk_c == 'O' || sk_c == 'o' || sk_c == 'U' || sk_c == 'u')
	{
		printf("Character \'%c\' is a VOWEL\n", sk_c);
	}
	else
	{
		sk_c_value = (int)sk_c;

		if ((sk_c_value >= UPPER_B && sk_c_value <= UPPER_E) || (sk_c_value >= LOWER_B && sk_c_value <= LOWER_E))
		{
			printf("Character \'%c\' is a CONSONANT\n", sk_c);
		}
		else if (sk_c_value >= DIGITS_B && sk_c_value <= DIGITS_E)
		{
			printf("Character \'%c\' is a DIGIT\n", sk_c);
		}
		else
		{
			printf("Character \'%c\' is a SPECIAL CHARACTER\n", sk_c);
		}
	}

	getch();
	return(0);

}
