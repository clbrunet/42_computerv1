TARGET = computor

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
DEPS = $(SRCS:.c=.d)

.PHONY: all
all: $(TARGET)

-include $(DEPS)

%.o: %.c
	$(CC) $(CFLAGS) -MMD -MP -MF $(<:.c=.d) -o $@ -c $<

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

.PHONY: clean
clean:
	rm -f $(OBJS) $(DEPS)

.PHONY: fclean
fclean: clean
	rm -f $(TARGET)

.PHONY: re
re: fclean all
