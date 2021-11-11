TARGET = computor

CC = clang
CFLAGS = -Wall -Wextra -Werror -I./src/ -MMD -MP

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
DEPS = $(SRCS:.c=.d)

all: $(TARGET)

-include $(DEPS)

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(CFLAGS) $(OBJS)

clean:
	rm -f $(OBJS) $(DEPS)

fclean: clean
	rm -f $(TARGET)

re: fclean all

.PHONY: all clean fclean re
