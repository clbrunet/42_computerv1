#include <stdio.h>
#include "computorv1/utils.h"
#include "computorv1/coefficient_node.h"
#include "computorv1/ast.h"
#include "computorv1/main.h"

coefficient_node *coefficient_node_new(int x_exponent, double value,
		coefficient_node *next)
{
	coefficient_node *new = malloc(sizeof(coefficient_node));
	if (new == NULL) {
		return NULL;
	}
	new->x_exponent = x_exponent;
	new->value = value;
	new->next = next;
	return new;
}

int get_reduced_term_x_exponent(ast_node *term)
{
	if (term->token == NUMBER) {
		return 0;
	}
	else if (term->token == VARIABLE) {
		return 1;
	}
	ast_node *iter = term;
	while (iter->token != EXPONENT) {
		if (iter->left->token != NUMBER) {
			iter = iter->left;
		} else {
			iter = iter->right;
		}
	}
	return iter->right->value;
}

double get_reduced_term_coefficient(ast_node *term)
{
	if (term->token == NUMBER) {
		return term->value;
	} else if (term->token == VARIABLE) {
		return 1;
	} else if (term->left->token == NUMBER) {
		return term->left->value;
	} else {
		return term->right->value;
	}
}

coefficient_node *search_x_exponent_node(coefficient_node *coefficients,
		int x_exponent)
{
	coefficient_node *iter = coefficients;
	while (iter) {
		if (iter->x_exponent == x_exponent) {
			return iter;
		}
		iter = iter->next;
	}
	return NULL;
}

coefficient_node *search_max_x_exponent_node(coefficient_node *coefficients)
{
	coefficient_node *iter = coefficients;
	coefficient_node *max_x_exponent_node = iter;
	int max_x_exponent_value = -1;
	while (iter) {
		if (iter->x_exponent > max_x_exponent_value) {
			max_x_exponent_node = iter;
			max_x_exponent_value = iter->x_exponent;
		}
		iter = iter->next;
	}
	return max_x_exponent_node;
}

int coefficient_node_push_back(coefficient_node **coefficients, int x_exponent, double value)
{
	if (*coefficients == NULL) {
		*coefficients = coefficient_node_new(x_exponent, value, NULL);
		if (*coefficients == NULL) {
			return -1;
		}
		return 0;
	}
	coefficient_node *iter = *coefficients;
	while (iter->next) {
		iter = iter->next;
	}
	iter->next = coefficient_node_new(x_exponent, value, NULL);
	if (iter->next == NULL) {
		return -1;
	}
	return 0;
}

void free_coefficients(coefficient_node *coefficients)
{
	coefficient_node *iter = coefficients;
	while (iter) {
		coefficient_node *to_free = iter;
		iter = iter->next;
		free(to_free);
	}
}

void coefficient_node_remove_node(coefficient_node **coefficients, coefficient_node *node)
{
	coefficient_node *iter = *coefficients;
	if (iter == node) {
		*coefficients = iter->next;
		free(node);
		return;
	}
	while (iter->next) {
		if (iter->next == node) {
			iter->next = node->next;
			free(node);
			return;
		}
		iter = iter->next;
	}
}

void coefficient_node_remove_x_eponent_node(coefficient_node **coefficients,
		int x_exponent)
{
	coefficient_node *iter = *coefficients;
	if (iter->x_exponent == x_exponent) {
		*coefficients = iter->next;
		free(iter);
		return;
	}
	while (iter->next) {
		if (iter->next->x_exponent == x_exponent) {
			coefficient_node *to_free = iter->next;
			iter->next = to_free->next;
			free(to_free);
			return;
		}
		iter = iter->next;
	}
}

ast_node *coefficient_node_to_ast_term(coefficient_node *node)
{
	ast_node *term = ast_node_new(NUMBER, node->value, NULL, NULL);
	if (term == NULL) {
		return NULL;
	}
	if (node->x_exponent > 0) {
		ast_node *variable = ast_node_new(VARIABLE, 0, NULL, NULL);
		ast_node *exponent_number = ast_node_new(NUMBER, node->x_exponent, NULL, NULL);
		ast_node *exponent = ast_node_new(EXPONENT, 0, variable, exponent_number);
		ast_node *multiplication = ast_node_new(MULTIPLICATION, 0, term, exponent);
		if (variable == NULL || exponent_number == NULL || exponent == NULL
			|| multiplication == NULL) {
			free(term);
			free(variable);
			free(exponent_number);
			free(exponent);
			free(multiplication);
			return NULL;
		}
		term = multiplication;
	}
	return term;
}

coefficient_node *get_expression_coefficients(ast_node *expression)
{
	coefficient_node *coefficients = NULL;
	ast_node *iter = expression;
	int i = 0;
	while (iter->token == ADDITION || iter->token == SUBSTRACTION) {
		i++;
		iter = iter->left;
	}
	int x_exponent = get_reduced_term_x_exponent(iter);
	double coefficient = get_reduced_term_coefficient(iter);
	if (is_number_zero(coefficient) == false) {
		coefficients = coefficient_node_new(x_exponent, coefficient, NULL);
		if (coefficients == NULL) {
			return NULL;
		}
	}
	while (i > 0) {
		i--;
		iter = expression;
		for (int j = i; j > 0; j--) {
			iter = iter->left;
		}
		token operation = iter->token;
		iter = iter->right;
		x_exponent = get_reduced_term_x_exponent(iter);
		coefficient = get_reduced_term_coefficient(iter);
		if (is_number_zero(coefficient)) {
			continue;
		}
		coefficient_node *x_exponent_node = search_x_exponent_node(coefficients, x_exponent);
		if (operation == SUBSTRACTION) {
			coefficient = -coefficient;
		}
		if (x_exponent_node) {
			x_exponent_node->value += coefficient;
			if (is_number_zero(x_exponent_node->value)) {
				coefficient_node_remove_node(&coefficients, x_exponent_node);
			}
		} else {
			if (coefficient_node_push_back(&coefficients, x_exponent, coefficient) == -1) {
				free_coefficients(coefficients);
				return NULL;
			}
		}
	}
	return coefficients;
}

