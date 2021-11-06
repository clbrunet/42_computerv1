#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include "computorv1/ast.h"
#include "computorv1/utils.h"
#include "computorv1/coefficient_node.h"

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
	ast_node *iter = equality->right;
	int i = 0;
	while (iter->token == ADDITION || iter->token == SUBSTRACTION) {
		i++;
		iter = iter->left;
	}
	ast_node *iter_dup = ast_node_dup(iter);
	if (iter_dup == NULL) {
		return -1;
	}
	ast_node *expression = ast_node_new(SUBSTRACTION, 0, equality->left, iter_dup);
	if (expression == NULL) {
		free_ast(iter_dup);
		return -1;
	}
	equality->left = expression;
	while (i > 0) {
		i--;
		iter = equality->right;
		for (int j = i; j > 0; j--) {
			iter = iter->left;
		}
		token operation = iter->token;
		iter = iter->right;
		iter_dup = ast_node_dup(iter);
		if (iter_dup == NULL) {
			return -1;
		}
		if (operation == SUBSTRACTION) {
			operation = ADDITION;
		} else {
			operation = SUBSTRACTION;
		}
		expression = ast_node_new(operation, 0, equality->left, iter_dup);
		if (expression == NULL) {
			free_ast(iter_dup);
			return -1;
		}
		equality->left = expression;
	}
	ast_node *zero = ast_node_new(NUMBER, 0, NULL, NULL);
	if (zero == NULL) {
		return -1;
	}
	free_ast(equality->right);
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

static int remove_zero(ast_node *operation, ast_node **parent_link, ast_node *zero, ast_node *zero_sibling)
{
	bool is_left = true;
	if (operation->right == zero) {
		is_left = false;
	}
	switch (operation->token) {
		case EXPONENT:
			if (operation->left->token == NUMBER && is_number_zero(operation->left->value)
				&& operation->right->token == NUMBER && is_number_zero(operation->right->value)) {
				zero_exponent_zero();
				return -1;
			}
			free(operation->left);
			free(operation->right);
			if (is_left) {
				set_ast_node(operation, NUMBER, 0, NULL, NULL);
			} else {
				set_ast_node(operation, NUMBER, 1, NULL, NULL);
			}
			break;
		case SUBSTRACTION:
			if (is_left) {
				operation->token = MULTIPLICATION;
				operation->left->value = -1;
				return 0;
			}
			// no break to execute addition case
		case ADDITION:
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

static int division_to_multiplication(ast_node *node)
{
	if (node == NULL) {
		return 0;
	}
	if (division_to_multiplication(node->left) == -1) {
		return -1;
	}
	if (division_to_multiplication(node->right) == -1) {
		return -1;
	}

	if (node->token == DIVISION && contains_variable(node->left)) {
		ast_node *one = ast_node_new(NUMBER, 1, NULL, NULL);
		ast_node *denominator_dup = ast_node_dup(node->right);
		if (one == NULL || denominator_dup == NULL) {
			free(one);
			free_ast(denominator_dup);
			return -1;
		}
		ast_node *new_division = ast_node_new(DIVISION, 0, one, denominator_dup);
		if (new_division == NULL) {
			free(one);
			free_ast(denominator_dup);
			return -1;
		}
		ast_node *numerator_dup = ast_node_dup(node->left);
		if (numerator_dup == NULL) {
			free_ast(new_division);
			return -1;
		}
		ast_node *multiplication = ast_node_new(MULTIPLICATION, 0,
				new_division, numerator_dup);
		if (multiplication == NULL) {
			free_ast(new_division);
			free_ast(numerator_dup);
			return -1;
		}
		if (ast_node_cpy(node, multiplication) == NULL) {
			free_ast(multiplication);
			return -1;
		}
		free_ast(multiplication);
	}
	return 0;
}

static int reduce_term(ast_node *term)
{
	if (term->token != MULTIPLICATION) {
		return 0;
	}
	while ((term->left->token == MULTIPLICATION && term->right->token == NUMBER)
		|| (term->right->token == MULTIPLICATION && term->left->token == NUMBER)) {
		if (term->right->token == MULTIPLICATION) {
			ast_node *tmp = term->left;
			term->left = term->right;
			term->right = tmp;
		}
		if (term->left->left->token == NUMBER) {
			term->left->left->value *= term->right->value;
		} else {
			term->left->right->value *= term->right->value;
		}
		ast_node *new_term_dup = ast_node_dup(term->left);
		if (new_term_dup == NULL) {
			return -1;
		}
		if (ast_node_cpy(term, new_term_dup) == NULL) {
			free_ast(new_term_dup);
			return -1;
		}
		free_ast(new_term_dup);
	}
	return 0;
}

static int reduce_terms(ast_node *expression)
{
	ast_node *iter = expression;
	int i = 0;
	while (iter->token == ADDITION || iter->token == SUBSTRACTION) {
		i++;
		iter = iter->left;
	}
	if (reduce_term(iter) == -1) {
		return -1;
	}
	while (i > 0) {
		i--;
		iter = expression;
		for (int j = i; j > 0; j--) {
			iter = iter->left;
		}
		iter = iter->right;
		if (reduce_term(iter) == -1) {
			return -1;
		}
	}
	return 0;
}

static int reduce_expression(ast_node *expression)
{
	if (expression->token == NUMBER) {
		return 0;
	}
	coefficient_node *coefficients = get_expression_coefficients(expression);
	if (coefficients == NULL) {
		return -1;
	}
	coefficient_node *max_x_exponent_node = search_max_x_exponent_node(coefficients);
	ast_node *reduced_expression = coefficient_node_to_ast_term(max_x_exponent_node);
	if (reduced_expression == NULL) {
		free_coefficients(coefficients);
		return -1;
	}
	coefficient_node_remove_node(&coefficients, max_x_exponent_node);
	while (coefficients) {
		max_x_exponent_node = search_max_x_exponent_node(coefficients);
		if (is_number_zero(max_x_exponent_node->value)) {
			coefficient_node_remove_node(&coefficients, max_x_exponent_node);
			continue;
		}
		bool is_substraction = false;
		if (max_x_exponent_node->value < 0) {
			is_substraction = true;
			max_x_exponent_node->value = -max_x_exponent_node->value;
		}
		ast_node *rhs = coefficient_node_to_ast_term(max_x_exponent_node);
		if (rhs == NULL) {
			free_coefficients(coefficients);
			free_ast(reduced_expression);
			return -1;
		}
		ast_node *new_reduced_expression = ast_node_new(
				(is_substraction) ? SUBSTRACTION : ADDITION, 0, reduced_expression, rhs);
		if (new_reduced_expression == NULL) {
			free_coefficients(coefficients);
			free_ast(reduced_expression);
			free_ast(rhs);
			return -1;
		}
		reduced_expression = new_reduced_expression;
		coefficient_node_remove_node(&coefficients, max_x_exponent_node);
	}
	if (ast_node_cpy(expression, reduced_expression) == NULL) {
		free_ast(reduced_expression);
		return -1;
	}
	free_ast(reduced_expression);
	return 0;
}

int reduce_ast(ast_node *equality)
{
	if (transfer_right_side(equality) == -1) {
		return -1;
	}
	if (remove_zeros(equality->left, &equality->left) == -1) {
		return -1;
	}
	if (division_to_multiplication(equality->left) == -1) {
		return -1;
	}
	if (leaf_number_operations(equality->left) == -1) {
		return -1;
	}
	if (reduce_terms(equality->left) == -1) {
		return -1;
	}
	if (reduce_expression(equality->left) == -1) {
		return -1;
	}
	return 0;
}
