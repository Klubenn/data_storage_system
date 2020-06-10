#ifndef STORAGE_H
#define STORAGE_H

enum
{
	ADD_UNIQUE,
	ADD_ELEM,
};

int system_init(int (**ext_delete_func)(void *), int (**ext_uniq_func)(void *, void *));
int system_destroy();
int add_element(void *data, int data_type, int uniqueness);
int delete_element(void *data, int data_type);
void iterate_elements(int data_type, void (**ext_iter_func)(void *));

#endif
