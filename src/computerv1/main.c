#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include "computerv1/style.h"
#include "computerv1/options.h"
#include "computerv1/parse_option_args.h"
#include "computerv1/ast.h"
#include "computerv1/parse_equation_arg.h"
#include "computerv1/reduce_ast.h"
#include "computerv1/solve_equation.h"

static void print_help(char *executable)
{
	printf(BOLD FG_RED "NAME\n" RESET_ALL);
	printf("\tcomputor - equation parser and solver\n");

	printf(BOLD FG_RED "\nUSAGE\n" RESET_ALL);
	printf(BOLD "\t%s" RESET_ALL " [" UNDERLINED "OPTION" RESET_UNDERLINED
			"]... " UNDERLINED "EQUATION" RESET_UNDERLINED "\n", executable);

	printf(BOLD FG_RED "\nOPTIONS\n" RESET_ALL);
	printf(BOLD FG_MAGENTA "\t-h," FG_DEFAULT " --help\n" RESET_ALL
			"\t\tPrint help information.\n");
	printf(BOLD FG_MAGENTA "\t-t," FG_DEFAULT " --tree\n" RESET_ALL
			"\t\tPrint equation tree.\n");
}

int main(int argc, char *argv[])
{
	if (argc == 1) {
		print_help(argv[0]);
		return 1;
	}
	options options = {0};
	if (parse_option_args(argc, argv, &options) == -1) {
		return 2;
	}
	if (options.help) {
		print_help(argv[0]);
		return 0;
	}
	ast_node *ast = parse_equation_arg(argv[argc - 1]);
	if (ast == NULL) {
		return 3;
	}
	if (options.tree) {
		print_ast(ast);
	} else {
		if (reduce_ast(ast) == -1) {
			free_ast(ast);
			return 4;
		}
		char *string = ast_to_string(ast, NULL);
		if (string == NULL) {
			free_ast(ast);
			return 5;
		}
		printf("Reduced form: %s\n", string);
		free(string);
		int polynomial_degree = get_reduced_expression_polynomial_degree(ast->left);
		printf("Polynomial degree: %d\n", polynomial_degree);
		switch (polynomial_degree) {
			case 0:
				solve_0_polynomial_degree_equation(ast);
				break;
			case 1:
				if (solve_1_polynomial_degree_equation(ast) == -1) {
					free_ast(ast);
					return 6;
				}
				break;
			case 2:
				if (solve_2_polynomial_degree_equation(ast) == -1) {
					free_ast(ast);
					return 6;
				}
				break;
			default:
				printf("The polynomial degree is stricly greater than 2, I can't solve.\n");
		}
	}
	free_ast(ast);
	return 0;
}
