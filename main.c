#include "storage.h"

/*
 * These are sample prototypes of external functions for deleting data,
 * checking it's uniqueness and iteration.
 * Sample structure is to show that the structure itself is passed and it's type.
 * storage.h needs to be included to use the libstorage library.
 */

typedef struct	s_struc
{
	int 	type;
	char 	*str;
}				t_struc;

void ext_iter_func(void *struc);

int ext_delete_func(int type, void *struc);

int ext_uniq_func(int type, void *struc1, void *struc2);

int main(int ac, char **av)
{
	t_struc *struc;

	system_init(&ext_delete_func, &ext_uniq_func);

	/* Pass ADD_ELEMENT or ADD_UNIQUE to add data anyway or only if it is unique */
	add_element(struc, struc->type, ADD_UNIQUE);
	delete_element(struc, struc->type);
	iterate_elements(struc->type, &ext_iter_func);
	system_destroy();
	return (0);
}
