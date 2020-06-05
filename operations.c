#include "sys_management.h"

static t_elem *new_element(void *data, int type)
{
	t_elem *new_elem;

	new_elem = (t_elem *)calloc(1, sizeof(t_elem));
	if (!new_elem)
		return (NULL);
	new_elem->type = type;
	new_elem->data = data;
	new_elem->next = NULL;
	return (new_elem);
}

static t_type *new_type(int data_type)
{
	t_type *type;

	type = (t_type *)calloc(1, sizeof(t_type));
	if (!type)
		return (NULL);
	type->elem_type = data_type;
	return (type);
}

static t_system *create_system(t_system *system, int (*ext_delete_func)(int, void *),
						 int (*ext_uniq_func)(int, void *, void *))
{
	t_system *new_system;

	if (!system)
	{
		new_system = (t_system *)calloc(1, sizeof(t_system));
		if (!new_system)
			return (NULL);
		new_system->ext_delete_func = ext_delete_func;
		new_system->ext_uniq_func = ext_uniq_func;
		return (new_system);
	}
	return (NULL);
}

static int destroy_system(t_system *system)
{
	t_type	*type;
	t_type	*type_tmp;
	t_elem	*elem;
	t_elem	*elem_tmp;
	int 	flag;

	flag = SUCCESS;
	if (!system)
		return (FAIL);
	type = system->first_type;
	while (type)
	{
		elem = type->first_elem;
		while(elem)
		{
			if (system->ext_delete_func(elem->type, elem->data) == FAIL)
				flag = FAIL;
			elem_tmp = elem->next;
			free(elem);
			elem = elem_tmp;
		}
		type_tmp = type->next;
		free(type);
		type = type_tmp;
	}
	free(system);
	system = NULL;
	return (flag);
}

static int put_element(t_system *system, t_type *type, void *data, int data_type, int uniquness)
{
	t_elem *elem;

	if (!type->first_elem)
	{
		type->first_elem = new_element(data, data_type);
		if (!type->first_elem)
			return (FAIL);
		type->last_elem = type->first_elem;
		return (SUCCESS);
	}
	if (uniquness == ADD_ELEM)
	{
		type->last_elem->next = new_element(data, data_type);
		if (!type->last_elem->next)
			return (FAIL);
		type->last_elem = type->last_elem->next;
		return (SUCCESS);
	}
	elem = type->first_elem;
	while (elem)
	{
		if (!system->ext_uniq_func(elem->type, elem->data, data))
			return (FAIL);
		if (!elem->next)
		{
			elem->next = new_element(data, data_type);
			if (!elem->next)
				return (FAIL);
			type->last_elem = elem;
			return (SUCCESS);
		}
		elem = elem->next;
	}
	return (FAIL); //redundant, because function will return inside the loop
}

static int element_creating(t_system *system, void *data, int data_type, int uniqueness)
{
	t_type *type;

	if (!system)
		return (FAIL);
	if (!system->first_type)
	{
		system->first_type = new_type(data_type);
		if (!system->first_type)
			return (FAIL);
	}
	type = system->first_type;
	while (type)
	{
		if (type->elem_type == data_type)
			return (put_element(system, type, data, data_type, uniqueness));
		if (!type->next)
		{
			type->next = new_type(data_type);
			if (!type->next)
				return (FAIL);
			return (put_element(system, type->next, data, data_type, uniqueness));
		}
		type = type->next;
	}
	return (FAIL); //redundant, because function will return inside the loop
}

static int element_deleting(t_system *system, void *data, int data_type)
{
	t_type *type;
	t_type *type_prev;
	t_elem *elem;
	t_elem *elem_prev;
	int 	flag;

	flag = FAIL;
	if (!system || !system->first_type)
		return (FAIL);
	type = system->first_type;
	type_prev = NULL;
	while (type)
	{
		if (type->elem_type == data_type)
		{
			elem = type->first_elem;
			elem_prev = NULL;
			while (elem)
			{
				if (elem->data == data)
				{
					system->ext_delete_func(elem->type, elem->data);
					flag = SUCCESS;
					if (elem->next)
					{
						if (elem_prev)
							elem_prev->next = elem->next;
						else
							type->first_elem = elem->next;
						elem_prev = elem;
						elem = elem->next;
						free(elem_prev);
						elem_prev = NULL;
						continue;
					}
					else
					{
						if (elem_prev)
						{
							elem_prev->next = NULL;
							type->last_elem = elem_prev;
						}
						else
						{
							if (type_prev)
								type_prev->next = type->next;
							else
								system->first_type = type->next;
							free(type);
							type = NULL;
						}
						free(elem);
						elem = NULL;
						break;
					}
				}
				elem_prev = elem;
				elem = elem->next;
			}
			break;
		}
		type_prev = type;
		type = type->next;
	}
	return (flag);
}

static void elements_processing(t_system *system, int data_type, void (*ext_iter_func)(void *))
{
	t_type *type;
	t_elem *elem;

	if (!system || !system->first_type)
		return ;
	type = system->first_type;
	while (type)
	{
		if (type->elem_type == data_type)
		{
			elem = type->first_elem;
			while (elem)
			{
				ext_iter_func(elem->data);
				elem = elem->next;
			}
			break;
		}
		type = type->next;
	}
}

static int record_maintaining(void *data, int data_type, int action,
		int (*ext_delete_func)(int, void *), int (*ext_uniq_func)(int, void *, void *),
		void (*ext_iter_func)(void *))
{
	static t_system *system = NULL;
	int 	flag;

	if (action == INIT_SYS)
	{
		system = create_system(system, ext_delete_func, ext_uniq_func);
		if (!system)
			return (FAIL);
		return (SUCCESS);
	}
	if (action == REMOVE_SYS)
	{
		flag = destroy_system(system);
		system = NULL;
		return (flag);
	}
	if (action == ADD_ELEM || action == ADD_UNIQUE)
		return (element_creating(system, data, data_type, action));
	if (action == DEL_ELEM)
		return (element_deleting(system, data, data_type));
	if (action == ITER_ELEM)
		elements_processing(system, data_type, ext_iter_func);
	return (FAIL);
}

int system_init(int (*ext_delete_func)(int, void *),
				int (*ext_uniq_func)(int, void *, void *))
{
	return (record_maintaining(NULL, 0, INIT_SYS, ext_delete_func, ext_uniq_func, NULL));
}

int system_destroy()
{
	return (record_maintaining(NULL, 0, REMOVE_SYS, NULL, NULL, NULL));
}

int add_element(void *data, int data_type, int uniqueness)
{
	if (!data || data_type == 0 || (uniqueness != ADD_UNIQUE && uniqueness != ADD_ELEM))
		return (FAIL);
	return (record_maintaining(data, data_type, uniqueness, NULL, NULL, NULL));
}

int delete_element(void *data, int data_type)
{
	if (!data || !data_type)
		return (FAIL);
	return (record_maintaining(data, data_type, DEL_ELEM, NULL, NULL, NULL));
}

void iterate_elements(int data_type, void (*ext_iter_func)(void *))
{
	if (data_type != 0)
		record_maintaining(NULL, data_type, ITER_ELEM, NULL, NULL, ext_iter_func);
}