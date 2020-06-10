#include <fcntl.h>
#include "storage.h"
#include "libft.h"
#include <stdio.h>

void iter_string(void *data)
{
	char 	*s;

	s = (char *)data;
	if (*s >= 'a' && *s<= 'z')
		*s -= 32;
	else if (*s >= 'A' && *s <= 'Z')
		*s += 32;
	else
		*s = '#';
}

void iter_int(void *data)
{
	int		*i;

	i = (int *)data;
	*i *= -1;
}

int delete_string(void *data)
{
	char *s0;
	char *s1;

	s0 = (char *)data;
	s1 = s0;
	while (*s1)
	{
		*s1 = 0;
		s1++;
	}
	free(s0);
	return (0);
}

int delete_int(void *data)
{
	int *i;

	i = (int *)data;
	*i = 0;
	free(i);
	return (0);
}

int unique_string(void *data1, void *data2)
{
	int i;

	if ((i = strcmp((char *)data1, (char *)data2)))
		return (i);
	return (0);
}

int unique_int(void *data1, void *data2)
{
	int *i1;
	int *i2;

	i1 = (int *)data1;
	i2 = (int *)data2;
	if (*i1 == *i2)
		return (0);
	return (1);
}

//typedef void (*iterate)(void *data);
//typedef int (*delete)(void *data);
//typedef int (*uniqueness)(void *data1, void *data2);


void (*ext_iter_func[])(void *) = {NULL, iter_string, iter_int};
int (*ext_delete_func[])(void *) = {NULL, delete_string, delete_int};
int (*ext_uniq_func[])(void *, void *) = {NULL, unique_string, unique_int};

int main(int ac, char **av)
{
	int *p_int[10];
	char *p_str[10];
	int status;

	for (int i = 0; i < 10; i++)
	{
		p_int[i] = (int *)malloc(sizeof(int));
		p_str[i] = (char *)malloc(sizeof(char) * 10);
		if (!p_int[i] || !p_str[i])
			return (0);
		*p_int[i] = 1111 * i;
		for (int j = 0; j < 9; j++)
			p_str[i][j] = 'a' + i;
		p_str[i][9] = '\0';
	}

	printf("SUCCESS is '0', FAIL is '1'\n\n");
	printf("Test 1: Double initializing, double deinitializing of system:\n");
	printf("[expected]-[result]\n");
	status = system_init(ext_delete_func, ext_uniq_func);
	printf("[0]-[%d] - initialization\n", status);
	status = system_init(ext_delete_func, ext_uniq_func);
	printf("[1]-[%d] - initialization\n", status);
	status = system_destroy();
	printf("[0]-[%d] - deinitialization\n", status);
	status = system_destroy();
	printf("[1]-[%d] - deinitialization\n", status);

	printf("\nTest 2: Initializing system, adding 10 new int and string elements:\n");
	status = system_init(ext_delete_func, ext_uniq_func);
	printf("%d - initialization\n", status);
	for (int i = 0; i < 10; i++)
	{
		status = add_element(p_int[i], 2, ADD_ELEM);
		if (i == 1)
			printf("add %dst int / string - %d", i, status);
		else if (i == 2 || i == 3)
			printf("add %dd  int / string - %d", i, status);
		else
			printf("add %dth int / string - %d", i, status);
		status = add_element(p_str[i], 1, ADD_ELEM);
		printf(" / %d\n", status);
	}

	printf("\nTest 3: Adding duplicate int and string elements with ADD_UNIQUE option:\n");
	printf("[expected]-[result]\n");
	int a = 7777;
	char *b = "ccccccccc";
	status = add_element(&a, 2, ADD_UNIQUE);
	printf("[1]-[%d] - duplicate int\n", status);
	status = add_element(b, 1, ADD_UNIQUE);
	printf("[1]-[%d] - duplicate string\n", status);

	printf("\nTest 4: Iterating and changing elements:\n");
	printf("Before changes\n%5s | %s\n", "int", "string");
	for (int i = 0; i < 10; i++)
		printf("%+05d | %s\n", *p_int[i], p_str[i]);
	iterate_elements(2, ext_iter_func);
	iterate_elements(1, ext_iter_func);
	printf("\nAfter changes\n%5s | %s\n", "int", "string");
	for (int i = 0; i < 10; i++)
		printf("%+05d | %s\n", *p_int[i], p_str[i]);

	printf("\nTest 5: Adding unchanged int and string elements with ADD_UNIQUE option:\n");
	printf("[expected]-[result]\n");
	status = add_element(&a, 2, ADD_UNIQUE);
	printf("[0]-[%d] - duplicate int\n", status);
	status = add_element(b, 1, ADD_UNIQUE);
	printf("[0]-[%d] - duplicate string\n", status);

	printf("\nTest 6: Removing one element. Expected undefined symbols for 4th element:\n");
	delete_element(p_int[4], 2);
	delete_element(p_str[4], 1);
	for (int i = 0; i < 11; i++)
		printf("%d: %+011d | %s\n", i, *p_int[i], p_str[i]);
	status = system_destroy();
	printf("%d - deinitialization\n", status);
	return (0);


//	system_init(&ext_delete_func, &ext_uniq_func);
//	add_element(struc, struc->type, ADD_ELEM | ADD_UNIQUE);
//	delete_element(struc, struc->type);
//	iterate_elements(struc->type, &ext_iter_func);
//	system_destroy();
}
