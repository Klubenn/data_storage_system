PROG = test_storage

LIB = libstorage.a

DIR = ./storage/

INCL_H = storage_internal.h
INCLUDES = $(addprefix $(DIR),$(INCL_H))

SRC_C = operations.c
SOURCE = $(addprefix $(DIR),$(SRC_C))

OBJECT = $(SOURCE:%.c=%.o)

FLAGS = -Wall -Wextra -Werror

all: $(LIB) $(PROG)

$(LIB): $(SOURCE) $(OBJECT)
	ar rc $(LIB) $(OBJECT)
	ranlib $(LIB)

%.o: %.c $(INCLUDES)
	gcc $(FLAGS) -I$(DIR) -o $@ -c $<

$(PROG): main_test.c storage.h $(LIB)
	gcc $(FLAGS) main_test.c -L. -lstorage -o $(PROG)

clean:
	rm -f $(OBJECT)

fclean: clean
	rm -f $(PROG) $(LIB)

re: fclean all

.PHONY: all clean fclean re
