NAME = libstorage.a

DIR = ./storage/

INCL_H = storage_internal.h
INCLUDES = $(addprefix $(DIR),$(INCL_H))

SRC_C = operations.c
SOURCE = $(addprefix $(DIR),$(SRC_C))

OBJECT = $(SOURCE:%.c=%.o)

FLAGS = -Wall -Wextra -Werror

all: $(NAME)

$(NAME): $(SOURCE) $(OBJECT)
	ar rc $(NAME) $(OBJECT)
	ranlib $(NAME)

%.o: %.c $(INCLUDES)
	gcc $(FLAGS) -I$(DIR) -o $@ -c $<

clean:
	rm -f $(OBJECT)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
