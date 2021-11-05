#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include "computerv1/ast.h"

static bool is_number_zero(double n)
{
	if (-0.0000001 < n && n < 0.0000001) {
		return true;
	}
	return false;
}

// static bool is_numbers_equals(double n1, double n2)
// {
// 	if (-0.0000001 < (n1 - n2) && (n1 - n2) < 0.0000001) {
// 		return true;
// 	}
// 	return false;
// }

static void division_by_zero()
{
	fprintf(stderr, "Division by 0 is undefined\n");
}

static void zero_exponent_zero()
{
	fprintf(stderr, "0^0 is undefined\n");
}

static int transfer_right_side(ast_node *equality)
{
	ast_node *substraction = ast_node_new(SUBSTRACTION, 0, equality->left, equality->right);
	ast_node *zero = ast_node_new(NUMBER, 0, NULL, NULL);
	if (substraction == NULL || zero == NULL) {
		free(substraction);
		free(zero);
		return -1;
	}
	equality->left = substraction;
	equality->right = zero;
	return 0;
}

static int leaf_number_operation(ast_node *operation)
{
	if (operation->left->token != NUMBER || operation->right->token != NUMBER) {
		return 0;
	}
	double result;
	switch (operation->token) {
		case EXPONENT:
			if (is_number_zero(operation->left->value)
				&& is_number_zero(operation->right->value)) {
				zero_exponent_zero();
				return -1;
			}
			result = 1;
			for (int i = 0; i < (int)operation->right->value; i++) {
				result *= operation->left->value;
			}
			break;
		case MULTIPLICATION:
			result = operation->left->value * operation->right->value;
			break;
		case DIVISION:
			if (is_number_zero(operation->right->value)) {
				division_by_zero();
				return -1;
			}
			result = operation->left->value / operation->right->value;
			break;
		case ADDITION:
			result = operation->left->value + operation->right->value;
			break;
		case SUBSTRACTION:
			result = operation->left->value - operation->right->value;
			break;
		default:
			return -1;
	}
	free(operation->left);
	free(operation->right);
	set_ast_node(operation, NUMBER, result, NULL, NULL);
	return 0;
}

static int leaf_number_operations(ast_node *node)
{
	if (node == NULL || node->token == NUMBER || node->token == VARIABLE) {
		return 0;
	}
	if (leaf_number_operations(node->left) == -1) {
		return -1;
	}
	if (leaf_number_operations(node->right) == -1) {
		return -1;
	}
	if (node->left->token == NUMBER && node->right->token == NUMBER) {
		if (leaf_number_operation(node) == -1) {
			return -1;
		}
	}
	return 0;
}

static int number_operations(ast_node *node)
{
	if (node == NULL || node->token == NUMBER || node->token == VARIABLE) {
		return 0;
	}
	if (number_operations(node->left) == -1) {
		return -1;
	}
	if (number_operations(node->right) == -1) {
		return -1;
	}
	if (token_precedence_compare(node->token, node->left->token) == 0
		&& node->right->token == NUMBER) {
		if (node->left->left->token == NUMBER || node->left->right->token == NUMBER) {
			token tmp = node->token;
			node->token = node->left->token;
			node->left->token = tmp;
		}
		if (node->left->left->token == NUMBER) {
			ast_node *tmp = node->right;
			node->right = node->left->right;
			node->left->right = tmp;
		} else if (node->left->right->token == NUMBER) {
			ast_node *tmp = node->right;
			node->right = node->left->left;
			node->left->left = node->left->right;
			node->left->right = tmp;
		}
		if (leaf_number_operation(node->left) == -1) {
			return -1;
		}
	}
	return 0;
}

static int remove_zero(ast_node *operation, ast_node **parent_link, ast_node *zero, ast_node *zero_sibling)
{
	bool is_left = true;
	if (operation->right == zero) {
		is_left = false;
	}
	switch (operation->token) {
		case EXPONENT:
			free(operation->left);
			free(operation->right);
			if (is_left) {
				set_ast_node(operation, NUMBER, 0, NULL, NULL);
			} else {
				set_ast_node(operation, NUMBER, 1, NULL, NULL);
			}
			break;
		case ADDITION:
		case SUBSTRACTION:
			*parent_link = zero_sibling;
			free(zero);
			free(operation);
			break;
		case DIVISION:
			if (is_left == false) {
				division_by_zero();
				return -1;
			}
			// no break to execute multiplication case
		case MULTIPLICATION:
			free_ast(operation->left);
			free_ast(operation->right);
			set_ast_node(operation, NUMBER, 0, NULL, NULL);
			break;
		default:
			return -1;
	}
	return 0;
}

static int remove_zeros(ast_node *operation, ast_node **parent_link)
{
	if (operation == NULL || operation->token == NUMBER || operation->token == VARIABLE) {
		return 0;
	}

	if (remove_zeros(operation->left, &operation->left) == -1) {
		return -1;
	}
	if (remove_zeros(operation->right, &operation->right) == -1) {
		return -1;
	}

	if (operation->left->token == NUMBER && is_number_zero(operation->left->value)) {
		if (remove_zero(operation, parent_link, operation->left, operation->right)) {
			return -1;
		}
	} else if (operation->right->token == NUMBER && is_number_zero(operation->right->value)) {
		if (remove_zero(operation, parent_link, operation->right, operation->left)) {
			return -1;
		}
	}
	return 0;
}

static int leaf_variable_operation(ast_node *operation)
{
	if (operation->left->token != EXPONENT || operation->left->left->token != VARIABLE
		|| operation->right->token != EXPONENT
		|| operation->right->left->token != VARIABLE) {
		return 0;
	}
	switch (operation->token) {
		case MULTIPLICATION:
			operation->right->value = operation->left->right->value
				+ operation->right->right->value;
			free_ast_childs(operation->left);
			free_ast_childs(operation->right);
			operation->token = EXPONENT;
			operation->left->token = VARIABLE;
			operation->right->token = NUMBER;
			break;
		case DIVISION:
			operation->right->value = operation->left->right->value
				- operation->right->right->value;
			free_ast_childs(operation->left);
			free_ast_childs(operation->right);
			operation->token = EXPONENT;
			operation->left->token = VARIABLE;
			operation->right->token = NUMBER;
			break;
		case ADDITION:
			if (operation->left->right->value == operation->right->right->value) {
				operation->token = MULTIPLICATION;
				free_ast_childs(operation->left);
				operation->left->token = NUMBER;
				operation->left->value = 2;
			} else if (operation->left->right->value < operation->right->right->value) {
				ast_node *tmp = operation->left;
				operation->left = operation->right;
				operation->right = tmp;
			}
			break;
		case SUBSTRACTION:
			if (operation->left->right->value == operation->right->right->value) {
				operation->token = NUMBER;
				operation->left->value = 0;
				free_ast_childs(operation);
			} else if (operation->left->right->value < operation->right->right->value) {
				ast_node *tmp = operation->left;
				operation->left = operation->right;
				operation->right = tmp;
			}
			break;
		default:
			return -1;
	}
	return 0;
}

static int leaf_variable_operations(ast_node *node)
{
	if (node == NULL || node->token == NUMBER || node->token == VARIABLE) {
		return 0;
	}
	if (leaf_variable_operations(node->left) == -1) {
		return -1;
	}
	if (leaf_variable_operations(node->right) == -1) {
		return -1;
	}
	if (node->left->token == EXPONENT && node->left->left->token == VARIABLE
		&& node->right->token == EXPONENT && node->right->left->token == VARIABLE) {
		if (leaf_variable_operation(node) == -1) {
			return -1;
		}
	}
	return 0;
}

static int variable_operation_mult_var(ast_node *operation)
{
	if (!((operation->left->token == EXPONENT && operation->left->left->token == VARIABLE
			&& operation->right->token == MULTIPLICATION
			&& operation->right->right->token == EXPONENT
			&& operation->right->right->left->token == VARIABLE)
		|| (operation->right->token == EXPONENT && operation->right->left->token == VARIABLE
			&& operation->left->token == MULTIPLICATION
			&& operation->left->right->token == EXPONENT
			&& operation->left->right->left->token == VARIABLE))) {
		return 0;
	}
	if (operation->left->token == MULTIPLICATION) {
		if (operation->left->right->right->value == operation->right->right->value) {
			operation->left->token = NUMBER;
			if (operation->token == ADDITION) {
				operation->left->value = operation->left->left->value + 1;
			} else if (operation->token == SUBSTRACTION) {
				operation->left->value = operation->left->left->value - 1;
			} else {
				return -1;
			}
			operation->token = MULTIPLICATION;
			free_ast_childs(operation->left);
		} else if (operation->left->right->right->value < operation->right->right->value) {
			ast_node *tmp = operation->left;
			operation->left = operation->right;
			operation->right = tmp;
		}
	} else {
		if (operation->right->right->right->value == operation->left->right->value) {
			operation->left->token = NUMBER;
			if (operation->token == ADDITION) {
				operation->left->value = 1 + operation->right->left->value;
			} else if (operation->token == SUBSTRACTION) {
				operation->left->value = 1 - operation->right->left->value;
			} else {
				return -1;
			}
			operation->token = MULTIPLICATION;
			free_ast_childs(operation->left);
			ast_node *exponent = operation->right->right;
			free(operation->right->left);
			free(operation->right);
			operation->right = exponent;
		} else if (operation->right->right->right->value < operation->left->right->value) {
			ast_node *tmp = operation->left;
			operation->left = operation->right;
			operation->right = tmp;
		}
	}
	return 0;
}

static int variable_operation_mult_mult(ast_node *operation)
{
	if (!((operation->left->token == MULTIPLICATION
			&& operation->left->right->token == EXPONENT
			&& operation->left->right->left->token == VARIABLE
			&& operation->right->token == MULTIPLICATION
			&& operation->right->right->token == EXPONENT
			&& operation->right->right->left->token == VARIABLE)
		|| (operation->right->token == MULTIPLICATION
			&& operation->right->right->token == EXPONENT
			&& operation->right->right->left->token == VARIABLE
			&& operation->left->token == MULTIPLICATION
			&& operation->left->right->token == EXPONENT
			&& operation->left->right->left->token == VARIABLE))) {
		return 0;
	}
	if (operation->left->right->right->value == operation->right->right->right->value) {
		operation->left->token = NUMBER;
		if (operation->token == ADDITION) {
			operation->left->value = operation->left->left->value
				+ operation->right->left->value;
		} else if (operation->token == SUBSTRACTION) {
			operation->left->value = operation->left->left->value
				- operation->right->left->value;
		} else {
			return -1;
		}
		operation->token = MULTIPLICATION;
		free_ast_childs(operation->left);
		ast_node *exponent = operation->right->right;
		free(operation->right->left);
		free(operation->right);
		operation->right = exponent;
	} else if (operation->left->right->right->value < operation->right->right->right->value) {
		ast_node *tmp = operation->left;
		operation->left = operation->right;
		operation->right = tmp;
	}
	return 0;
}

static int variable_operations(ast_node *node)
{
	if (node == NULL || node->token == NUMBER || node->token == VARIABLE) {
		return 0;
	}
	if (variable_operations(node->left) == -1) {
		return -1;
	}
	if (variable_operations(node->right) == -1) {
		return -1;
	}

	if (token_precedence_compare(node->token, node->left->token) == 0
		&& node->right->token == EXPONENT && node->right->left->token == VARIABLE
		&& ((node->left->left->token == EXPONENT && node->left->left->left->token == VARIABLE)
			|| (node->left->right->token == EXPONENT
				&& node->left->right->left->token == VARIABLE))) {
		token tmp = node->token;
		node->token = node->left->token;
		node->left->token = tmp;
		if (node->left->left->token == EXPONENT && node->left->left->left->token == VARIABLE
			&& node->left->right->token == EXPONENT
			&& node->right->right->value < node->left->right->right->value) {
			ast_node *tmp = node->right;
			node->right = node->left->right;
			node->left->right = tmp;
		} else if (node->left->left->token == EXPONENT
			&& node->right->right->value < node->left->left->right->value) {
			ast_node *tmp = node->right;
			node->right = node->left->left;
			node->left->left = node->left->right;
			node->left->right = tmp;
		}
		if (leaf_variable_operation(node->left) == -1) {
			return -1;
		}
	}
	if ((node->left->token == EXPONENT && node->left->left->token == VARIABLE
			&& node->right->token == MULTIPLICATION
			&& node->right->right->token == EXPONENT
			&& node->right->right->left->token == VARIABLE)
		|| (node->right->token == EXPONENT && node->right->left->token == VARIABLE
			&& node->left->token == MULTIPLICATION
			&& node->left->right->token == EXPONENT
			&& node->left->right->left->token == VARIABLE)) {
		if (variable_operation_mult_var(node) == -1) {
			return -1;
		}
	}
	if ((node->left->token == MULTIPLICATION && node->left->right->token == EXPONENT
			&& node->left->right->left->token == VARIABLE
			&& node->right->token == MULTIPLICATION
			&& node->right->right->token == EXPONENT
			&& node->right->right->left->token == VARIABLE)
		|| (node->right->token == MULTIPLICATION
			&& node->right->right->token == EXPONENT
			&& node->right->right->left->token == VARIABLE
			&& node->left->token == MULTIPLICATION
			&& node->left->right->token == EXPONENT
			&& node->left->right->left->token == VARIABLE)) {
		if (variable_operation_mult_mult(node) == -1) {
			return -1;
		}
	}
	return 0;
}

ast_node *g_equality = NULL;

static int remove_parenthesis_left_term(ast_node *node, bool is_left)
{
	printf("node\n");
	print_ast(node);
	ast_node *expression = NULL;
	ast_node *factor = (!is_left) ? node->left : node->right;
	ast_node *iter = (is_left) ? node->left : node->right;
	int i = 0;
	while (iter->token != MULTIPLICATION && iter->token != DIVISION
			 && iter->token != NUMBER && iter->token != EXPONENT) {
		i++;
		iter = iter->left;
	}
	ast_node *factor_dup = ast_node_dup(factor);
	if (factor_dup == NULL) {
		return -1;
	}
	ast_node *iter_dup = ast_node_dup(iter);
	if (iter_dup == NULL) {
		free_ast(factor_dup);
		return -1;
	}
	expression = ast_node_new(node->token, 0, factor_dup, iter_dup);
	if (expression == NULL) {
		free_ast(factor_dup);
		free_ast(iter_dup);
		return -1;
	}
	printf("expression\n");
	print_ast(expression);
	// exit(0);
	while (i > 0) {
		i--;
		ast_node *operation = node;
		iter = (is_left) ? node->left : node->right;
		for (int j = i; j > 0; j--) {
			operation = iter;
			iter = iter->left;
		}
		operation = iter;
		iter = iter->right;
		factor_dup = ast_node_dup((!is_left) ? node->left : node->right);
		if (factor_dup == NULL) {
			return -1;
		}
		iter_dup = ast_node_dup(iter);
		if (iter_dup == NULL) {
			free_ast(factor_dup);
			return -1;
		}
		ast_node *rhs = ast_node_new(node->token, 0, iter_dup, factor_dup);
		if (rhs == NULL) {
			free_ast(factor_dup);
			free_ast(iter_dup);
			return -1;
		}
		ast_node *new_expression = ast_node_new(operation->token, 0, expression, rhs);
		if (new_expression == NULL) {
			free_ast(rhs);
			free_ast(expression);
			return -1;
		}
		expression = new_expression;
	}
	if (ast_node_cpy(node, expression) == NULL) {
		free_ast(expression);
		return -1;
	}
	free_ast(expression);
	printf("new node\n");
	print_ast(node);
	return 0;
}

static int remove_parenthesis_right_term(ast_node *node, ast_node *parent, bool is_left)
{
	ast_node *expression = NULL;
	ast_node *factor = (!is_left) ? node->left : node->right;
	ast_node *iter = (is_left) ? node->left : node->right;
	int i = 0;
	while (iter->token != MULTIPLICATION && iter->token != DIVISION
			 && iter->token != NUMBER && iter->token != EXPONENT) {
		i++;
		iter = iter->left;
	}
	ast_node *factor_dup = ast_node_dup(factor);
	if (factor_dup == NULL) {
		return -1;
	}
	ast_node *iter_dup = ast_node_dup(iter);
	if (iter_dup == NULL) {
		free_ast(factor_dup);
		return -1;
	}
	ast_node *lhs_dup = ast_node_dup(parent->left);
	if (lhs_dup == NULL) {
		free_ast(factor_dup);
		free_ast(iter_dup);
		return -1;
	}
	expression = ast_node_new(parent->token, 0, lhs_dup, NULL);
	if (expression == NULL) {
		free_ast(factor_dup);
		free_ast(lhs_dup);
		free_ast(iter_dup);
		return -1;
	}
	expression->right = ast_node_new(node->token, 0, factor_dup, iter_dup);
	if (expression->right == NULL) {
		free_ast(expression);
		free_ast(factor_dup);
		free_ast(iter_dup);
		return -1;
	}
	while (i > 0) {
		i--;
		iter = (is_left) ? node->left : node->right;
		for (int j = i; j > 0; j--) {
			iter = iter->left;
		}
		ast_node *operation = iter;
		iter = iter->right;
		factor_dup = ast_node_dup((!is_left) ? node->left : node->right);
		if (factor_dup == NULL) {
			return -1;
		}
		iter_dup = ast_node_dup(iter);
		if (iter_dup == NULL) {
			free_ast(factor_dup);
			return -1;
		}
		ast_node *rhs = ast_node_new(node->token, 0, factor_dup, iter_dup);
		if (rhs == NULL) {
			free_ast(factor_dup);
			free_ast(iter_dup);
			return -1;
		}
		ast_node *new_expression = ast_node_new(operation->token, 0, expression, rhs);
		if (new_expression == NULL) {
			free_ast(rhs);
			free_ast(expression);
			return -1;
		}
		expression = new_expression;
	}
	if (ast_node_cpy(parent, expression) == NULL) {
		free_ast(expression);
		return -1;
	}
	free_ast(expression);
	return 0;
}

static int remove_parentheses(ast_node *node, ast_node *parent)
{
	if (node == NULL || node->token == NUMBER || node->token == VARIABLE) {
		return 0;
	}
	if (remove_parentheses(node->left, node) == -1) {
		return -1;
	}
	if (remove_parentheses(node->right, node) == -1) {
		return -1;
	}

	if (token_precedence_compare(node->left->token, node->token) < 0) {
		if (node == parent->left) {
			if (remove_parenthesis_left_term(node, true) == -1) {
				return -1;
			}
		} else {
			if (remove_parenthesis_right_term(node, parent, true) == -1) {
				return -1;
			}
		}
		if (remove_parentheses(parent->left, parent) == -1) {
			return -1;
		}
		return remove_parentheses(parent->right, parent);
	}
	if (node->right->token == MULTIPLICATION && node->token == MULTIPLICATION) {
		ast_node *tmp = node->left;
		node->left = node->right;
		node->right = tmp;
	}
	if (token_precedence_compare(node->right->token, node->token) <= 0
		&& node->token != DIVISION) {
		if (node->token == ADDITION || node->token == SUBSTRACTION) {
			ast_node *factor = ast_node_new(NUMBER,
					(node->token == SUBSTRACTION) ? -1 : 1, NULL, NULL);
			if (factor == NULL) {
				return -1;
			}
			ast_node *multiplication = ast_node_new(MULTIPLICATION, 0, factor, node->right);
			if (multiplication == NULL) {
				free(factor);
				return -1;
			}
			node->right = multiplication;
			node->token = ADDITION;
			parent = node;
			node = node->right;
		}
		if (node == parent->left) {
			if (remove_parenthesis_left_term(node, false) == -1) {
				return -1;
			}
		} else {
			if (remove_parenthesis_right_term(node, parent, false) == -1) {
				return -1;
			}
		}
		if (remove_parentheses(parent->left, parent) == -1) {
			return -1;
		}
		return remove_parentheses(parent->right, parent);
	}
	return 0;
}

static int check_expression(ast_node *node)
{
	ast_node *iter = node;
	int i = 0;
	while (iter->token == ADDITION || iter->token == SUBSTRACTION) {
		i++;
		iter = iter->left;
	}
	while (i > 0) {
		i--;
		iter = node;
		for (int j = i; j > 0; j--) {
			iter = iter->left;
		}
		iter = iter->right;
	}
	return 0;
}

int reduce_ast(ast_node *equality)
{
	// @todo to remove
	g_equality = equality;
	// char *string = ast_to_string(iter, NULL);
	// printf("%s\n", string);
	// free(string);
	if (transfer_right_side(equality) == -1) {
		return -1;
	}
	if (check_expression(equality->left) == -1) {
		return -1;
	}
	if (remove_zeros(equality->left, &equality->left) == -1) {
		return -1;
	}
	if (leaf_number_operations(equality->left) == -1) {
		return -1;
	}
	if (number_operations(equality->left) == -1) {
		return -1;
	}
	if (remove_parentheses(equality->left, equality) == -1) {
		return -1;
	}
	if (leaf_number_operations(equality->left) == -1) {
		return -1;
	}
	if (number_operations(equality->left) == -1) {
		return -1;
	}
	if (remove_zeros(equality->left, &equality->left) == -1) {
		return -1;
	}
	if (leaf_variable_operations(equality->left) == -1) {
		return -1;
	}
	// print_ast(equality->left);
	// printf("\n\n\n");
	// print_ast(equality->left);
	return 0;
}
