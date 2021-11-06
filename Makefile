NAME = computor

CC = clang
CFLAGS = -Wall -Wextra -Werror -I./src/

SRCS = src/computorv1/main.c \
			 src/computorv1/utils.c \
			 src/computorv1/ast.c \
			 src/computorv1/parse_equation_arg.c \
			 src/computorv1/parse_option_args.c \
			 src/computorv1/coefficient_node.c \
			 src/computorv1/reduce_ast.c \
			 src/computorv1/solve_equation.c \
			 src/computorv1/strings.c

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
