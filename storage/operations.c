#include "storage_internal.h"


static t_elem *new_element(void *data, int type, t_elem *prev)
{
	t_elem *new_elem;

	new_elem = (t_elem *)calloc(1, sizeof(t_elem));
	if (!new_elem)
		return (NULL);
	new_elem->type = type;
	new_elem->data = data;
	new_elem->prev = prev;
	return (new_elem);
}

static t_type *new_type(int data_type, t_type *prev)
{
	t_type *type;

	type = (t_type *)calloc(1, sizeof(t_type));
	if (!type)
		return (NULL);
	type->elem_type = data_type;
	type->prev = prev;
	return (type);
}

static void remove_duplicates(t_elem *elem)
{
	t_elem *next_elems;

	next_elems = elem->next;
	while (next_elems)
	{
		if (elem->data == next_elems->data)
			next_elems->data = NULL;
		next_elems = next_elems->next;
	}
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
			if (elem->data)
			{
				remove_duplicates(elem);
				if (system->ext_delete_func[elem->type](elem->data) == FAIL)
					flag = FAIL;
			}
			elem_tmp = elem->next;
			free(elem);
			elem = elem_tmp;
		}
		type_tmp = type->next;
		free(type);
		type = type_tmp;
	}
	return (flag);
}

static int put_element(t_system *system, t_type *type, void *data, int uniqueness)
{
	t_elem *elem;

	if (!type->first_elem)
	{
		type->first_elem = new_element(data, type->elem_type, NULL);
		if (!type->first_elem)
			return (FAIL);
		type->last_elem = type->first_elem;
		return (SUCCESS);
	}
	if (uniqueness == ADD_ELEM)
	{
		type->last_elem->next = new_element(data, type->elem_type, type->last_elem);
		if (!type->last_elem->next)
			return (FAIL);
		type->last_elem = type->last_elem->next;
		return (SUCCESS);
	}
	elem = type->first_elem;// if (uniqueness == ADD_UNIQUE)
	while (elem)
	{
		if (!system->ext_uniq_func[elem->type](elem->data, data))
			return (FAIL);
		if (!elem->next)
		{
			elem->next = new_element(data, type->elem_type, elem);
			if (!elem->next)
				return (FAIL);
			type->last_elem = elem->next;
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
		system->first_type = new_type(data_type, NULL);
		if (!system->first_type)
			return (FAIL);
	}
	type = system->first_type;
	while (type)
	{
		if (type->elem_type == data_type)
			return (put_element(system, type, data, uniqueness));
		if (!type->next)
		{
			type->next = new_type(data_type, type);
			if (!type->next)
				return (FAIL);
			return (put_element(system, type->next, data, uniqueness));
		}
		type = type->next;
	}
	return (FAIL); //redundant, because function will return inside the loop
}

static void type_deleting(t_system *system, t_type *type)
{
	if (type->next)
	{
		if (type->prev)
		{
			type->prev->next = type->next;
			type->next->prev = type->prev;
		}
		else
		{
			system->first_type = type->next;
			type->next->prev = NULL;
		}
	}
	else
	{
		if (type->prev)
			type->prev->next = NULL;
		else
			system->first_type = NULL;
	}
	free(type);
}

static int element_deleting_delete(t_system *system, t_type *type, t_elem *elem, int flag)
{
	int 	result;

	result = SUCCESS;
	if (!flag)
		result = system->ext_delete_func[elem->type](elem->data);
	if (elem->next)
	{
		if (elem->prev)
		{
			elem->prev->next = elem->next;
			elem->next->prev = elem->prev;
		}
		else
		{
			type->first_elem = elem->next;
			elem->next->prev = NULL;
		}
	}
	else if (elem->prev)
	{
		elem->prev->next = NULL;
		type->last_elem = elem->prev;
	}
	else
		type_deleting(system, type);
	free(elem);

	return (result);
}

static int element_deleting_find(t_system *system, void *data, int data_type)
{
	t_type *type;
	t_elem *elem;
	t_elem *elem_tmp;
	int 	result;
	int		flag;

	result = SUCCESS;
	flag = 0;
	type = system->first_type;
	while (type)
	{
		if (type->elem_type == data_type)
		{
			elem = type->first_elem;
			while (elem)
			{
				elem_tmp = elem->next;
				if (elem->data == data)
				{
					result = (element_deleting_delete(system, type, elem, flag) ? FAIL : result);
					flag++;
				}
				elem = elem_tmp;
			}
			break;
		}
		type = type->next;
	}
	if (flag)
		return (result);
	return (FAIL);
}

static void elements_iterating(t_system *system, int data_type, void (**ext_iter_func)(void *))
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
				ext_iter_func[elem->type](elem->data);
				elem = elem->next;
			}
			break;
		}
		type = type->next;
	}
}

static t_system *system_storage(int action)
{
	static t_system *system = NULL;

	if (action == REMOVE_SYS)
	{
		free(system);
		system = NULL;
	}
	else if (action == INIT_SYS)
	{
		if (system)
			return (NULL);
		system = (t_system *)calloc(1, sizeof(t_system));
		return (system);
	}
	return (system);
}

int system_init(int (**ext_delete_func)(void *), int (**ext_uniq_func)(void *, void *))
{
	t_system *system;

	if (!(ext_delete_func) || !(ext_uniq_func))
		return (FAIL);
	if (!(system = system_storage(INIT_SYS)))
		return (FAIL);
	system->ext_delete_func = ext_delete_func;
	system->ext_uniq_func = ext_uniq_func;
	return (SUCCESS);
}

int system_destroy()
{
	t_system *system;
	int 	result;

	if (!(system = system_storage(GET_DATA)))
		return (FAIL);
	result = destroy_system(system);
	system_storage(REMOVE_SYS);
	return (result);
}

int add_element(void *data, int data_type, int uniqueness)
{
	t_system *system;

	if (!data || data_type == 0 || (uniqueness != ADD_UNIQUE && uniqueness != ADD_ELEM))
		return (FAIL);
	if (!(system = system_storage(GET_DATA)))
		return (FAIL);
	return (element_creating(system, data, data_type, uniqueness));
}

int delete_element(void *data, int data_type)
{
	t_system *system;

	if (!data || !data_type)
		return (FAIL);
	if (!(system = system_storage(GET_DATA)))
		return (FAIL);
	return (element_deleting_find(system, data, data_type));
}

void iterate_elements(int data_type, void (**ext_iter_func)(void *))
{
	t_system *system;

	if (!data_type || !(ext_iter_func))
		return;
	if (!(system = system_storage(GET_DATA)))
		return;
	elements_iterating(system, data_type, ext_iter_func);
}
