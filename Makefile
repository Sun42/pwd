NAME = pwd
SRC = pwd.c
OBJ = $(SRC:.c=.o)
CFLAGS = -I/u/all/sundas_c/rendu/include -I.
INCLUDES = -L/u/all/sundas_c/rendu/lib/my -lmy
RM = rm -f
CC = gcc

$(NAME): $(OBJ)
	$(CC) -o $(NAME) $(OBJ) $(INCLUDES)

all: $(NAME)

clean :
	$(RM) $(OBJ)
	$(RM) *~

fclean: clean
	$(RM) $(NAME)

.PHONY: all clean fclean re
