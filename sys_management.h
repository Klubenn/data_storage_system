#ifndef SYS_MANAGEMENT_H
#define SYS_MANAGEMENT_H

#include <stdlib.h>

#define FAIL		1
#define SUCCESS		0

enum
{
	INIT_SYS,
	REMOVE_SYS,
	ADD_UNIQUE,
	ADD_ELEM,
	DEL_ELEM,
	ITER_ELEM,
};

typedef struct		s_elem
{
	int				type;
	void			*data;
	struct s_elem	*next;
}					t_elem;

typedef struct		s_type
{
	int 			elem_type;
	t_elem			*first_elem;
	t_elem			*last_elem;
	struct s_type	*next;
}					t_type;

typedef struct		s_system
{
	t_type			*first_type;
	int 			(*ext_delete_func)(int, void *);
	int 			(*ext_uniq_func)(int, void *, void *);
}					t_system;

int system_init(int (*ext_delete_func)(int, void *), int (*ext_uniq_func)(int, void *, void *));
int system_destroy();
int add_element(void *data, int data_type, int uniqueness);
int delete_element(void *data, int data_type);
void iterate_elements(int data_type, void (*ext_iter_func)(void *));
#endif
