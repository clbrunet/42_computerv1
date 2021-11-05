#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include "computerv1/style.h"
#include "computerv1/ast.h"
#include "computerv1/strings.h"

static bool is_integer(double n)
{
	if (-0.0000001 < (n - (int)n) && (n - (int)n) < 0.0000001) {
		return true;
	}
	return false;
}

static void skip_spaces(char *arg, int *index_ptr)
{
	while (arg[*index_ptr] == ' ') {
		(*index_ptr)++;
	}
}

static ast_node *parse_expression(char *arg, int *index_ptr);

static ast_node *parse_factor(char *arg, int *index_ptr)
{
	skip_spaces(arg, index_ptr);
	ast_node *factor = NULL;
	if (arg[*index_ptr] == '-' && (arg[*index_ptr + 1] == 'X'
			|| arg[*index_ptr + 1] == '(')) {
		(*index_ptr)++;
		factor = ast_node_new(NUMBER, -1, NULL, NULL);
	} else if (arg[*index_ptr] == 'X') {
		(*index_ptr)++;
		factor = ast_node_new(VARIABLE, 0, NULL, NULL);
	} else if (arg[*index_ptr] == '(') {
		(*index_ptr)++;
		factor = parse_expression(arg, index_ptr);
		if (arg[*index_ptr] != ')') {
			free_ast(factor);
			return NULL;
		}
		(*index_ptr)++;
	} else {
		char *endptr = NULL;
		double number = strtod(arg + *index_ptr, &endptr);
		if (errno == ERANGE || arg + *index_ptr == endptr) {
			return NULL;
		}
		*index_ptr = endptr - arg;
		factor = ast_node_new(NUMBER, number, NULL, NULL);
	}
	skip_spaces(arg, index_ptr);
	return factor;
}

static ast_node *parse_exponent(char *arg, int *index_ptr)
{
	ast_node *exponent = parse_factor(arg, index_ptr);
	if (exponent == NULL) {
		return NULL;
	}
	while (arg[*index_ptr] == '^') {
		(*index_ptr)++;
		int rhs_index = *index_ptr;
		ast_node *rhs = parse_factor(arg, index_ptr);
		if (rhs == NULL) {
			free_ast(exponent);
			return NULL;
		}
		if (rhs->token != NUMBER || !is_integer(rhs->value)) {
			*index_ptr = rhs_index;
			free_ast(exponent);
			free_ast(rhs);
			return NULL;
		}
		ast_node *new_exponent = ast_node_new(EXPONENT, 0, exponent, rhs);
		if (new_exponent == NULL) {
			free_ast(exponent);
			free_ast(rhs);
			return NULL;
		}
		exponent = new_exponent;
	}
	if (exponent->token == VARIABLE) {
		ast_node *one = ast_node_new(NUMBER, 1, NULL, NULL);
		if (one == NULL) {
			free(exponent);
			return NULL;
		}
		ast_node *new_exponent = ast_node_new(EXPONENT, 0, exponent, one);
		if (new_exponent == NULL) {
			free(exponent);
			free(one);
			return NULL;
		}
		exponent = new_exponent;
	}
	return exponent;
}

static ast_node *parse_term(char *arg, int *index_ptr)
{
	ast_node *term = parse_exponent(arg, index_ptr);
	if (term == NULL) {
		return NULL;
	}
	while (arg[*index_ptr] == '*' || arg[*index_ptr] == '/'
		|| arg[*index_ptr] == 'X' || arg[*index_ptr] == '(') {
		char operation = arg[*index_ptr];
		if (arg[*index_ptr] == '*' || arg[*index_ptr] == '/') {
			(*index_ptr)++;
		}
		ast_node *rhs = parse_exponent(arg, index_ptr);
		if (rhs == NULL) {
			free_ast(term);
			return NULL;
		}
		ast_node *new_term = NULL;
		if (operation == '/') {
			new_term = ast_node_new(DIVISION, 0, term, rhs);
		} else {
			new_term = ast_node_new(MULTIPLICATION, 0, term, rhs);
		}
		if (new_term == NULL) {
			free_ast(term);
			free_ast(rhs);
			return NULL;
		}
		term = new_term;
	}
	return term;
}

static ast_node *parse_expression(char *arg, int *index_ptr)
{
	ast_node *expression = parse_term(arg, index_ptr);
	if (expression == NULL) {
		return NULL;
	}
	while (arg[*index_ptr] == '+' || arg[*index_ptr] == '-') {
		char operation = arg[*index_ptr];
		(*index_ptr)++;
		ast_node *rhs = parse_term(arg, index_ptr);
		if (rhs == NULL) {
			free_ast(expression);
			return NULL;
		}
		ast_node *new_expression = NULL;
		if (operation == '+') {
			new_expression = ast_node_new(ADDITION, 0, expression, rhs);
		} else {
			new_expression = ast_node_new(SUBSTRACTION, 0, expression, rhs);
		}
		if (new_expression == NULL) {
			free_ast(expression);
			free_ast(rhs);
			return NULL;
		}
		expression = new_expression;
	}
	return expression;
}

static ast_node *parse_equation(char *arg, int *index_ptr)
{
	ast_node *equation = ast_node_new(EQUALITY, 0, NULL, NULL);
	if (equation == NULL) {
		return NULL;
	}
	equation->left = parse_expression(arg, index_ptr);
	if (equation->left == NULL) {
		free_ast(equation);
		return NULL;
	}
	if (arg[*index_ptr] != '=') {
		free_ast(equation);
		return NULL;
	}
	(*index_ptr)++;
	equation->right = parse_expression(arg, index_ptr);
	if (equation->right == NULL) {
		free_ast(equation);
		return NULL;
	}
	if (arg[*index_ptr] != '\0') {
		free_ast(equation);
		return NULL;
	}
	return equation;
}

bool has_variables_in_parenthesis(char *arg)
{
	int parenthesis = 0;
	while (*arg != '\0') {
		if (*arg == '(') {
			parenthesis++;
		} else if (*arg == ')') {
			parenthesis--;
		} else if (*arg == 'X') {
			if (parenthesis > 0) {
				return true;
			}
		}
		arg++;
	}
	return false;
}

static bool contain_variable(ast_node *node)
{
	if (node == NULL) {
		return false;
	}
	if (node->token == VARIABLE) {
		return true;
	}
	if (contain_variable(node->left)) {
		return true;
	}
	if (contain_variable(node->right)) {
		return true;
	}
	return false;
}

bool has_division_by_variables(ast_node *node)
{
	if (node == NULL) {
		return false;
	}
	if (node->token == DIVISION && contain_variable(node->right)) {
		return true;
	}
	if (has_division_by_variables(node->left)) {
		return true;
	}
	if (has_division_by_variables(node->right)) {
		return true;
	}
	return false;
}

ast_node *parse_equation_arg(char *arg)
{
	char *unexpected_token = strfind_first_not_of(arg, " =+-*/^0123456789.X()");
	if (unexpected_token != NULL) {
		fprintf(stderr, "Equation unexpected token : '%c'\n", *unexpected_token);
		return NULL;
	}
	int index = 0;
	ast_node *equation = parse_equation(arg, &index);
	if (equation == NULL) {
		if (errno == ENOMEM) {
			perror("malloc");
		} else if (errno == ERANGE) {
			perror("strtod");
		} else {
			fprintf(stderr, "Equation format error\n");
			printf("%s\n", arg);
			printf(BOLD FG_GREEN "%*c\n" RESET_ALL, index + 1, '^');
		}
		return NULL;
	}
	if (has_variables_in_parenthesis(arg)) {
		fprintf(stderr, "Does not support variable in parenthesis\n");
		free_ast(equation);
		return NULL;
	} else if (has_division_by_variables(equation)) {
		fprintf(stderr, "Does not support division by variables.\n");
		free_ast(equation);
		return NULL;
	}
	return equation;
}
