NAME = computer

CC = clang
CFLAGS = -Wall -Wextra -Werror -I./src/ -O0 -ggdb -Wno-unused-function -fsanitize=address

SRCS = src/computerv1/main.c \
			 src/computerv1/ast.c \
			 src/computerv1/parse_equation_arg.c \
			 src/computerv1/parse_option_args.c \
			 src/computerv1/reduce_ast.c \
			 src/computerv1/strings.c

OBJS = $(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) -o $(NAME) $(CFLAGS) $(OBJS)

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
