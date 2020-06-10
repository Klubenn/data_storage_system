#ifndef SYS_MANAGEMENT_H
#define SYS_MANAGEMENT_H

#include <stdlib.h>

#define FAIL		1
#define SUCCESS		0

enum
{
	ADD_UNIQUE,
	ADD_ELEM,
	INIT_SYS,
	REMOVE_SYS,
	GET_DATA,
};

typedef struct		s_elem
{
	int				type;
	void			*data;
	struct s_elem	*next;
	struct s_elem	*prev;
}					t_elem;

typedef struct		s_type
{
	int 			elem_type;
	t_elem			*first_elem;
	t_elem			*last_elem;
	struct s_type	*next;
	struct s_type	*prev;
}					t_type;

typedef struct		s_system
{
	t_type			*first_type;
	int 			(**ext_delete_func)(void *);
	int 			(**ext_uniq_func)(void *, void *);
}					t_system;

#endif
