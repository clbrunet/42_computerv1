#ifndef COMPUTERV1_COEFFICIENT_NODE_H
#define COMPUTERV1_COEFFICIENT_NODE_H

#include <stdlib.h>
#include "computerv1/ast.h"

typedef struct coefficient_node {
	int x_exponent;
	double value;
	struct coefficient_node *next;
} coefficient_node;

coefficient_node *coefficient_node_new(int x_exponent, double value,
		coefficient_node *next);

int get_reduced_term_x_exponent(ast_node *term);
double get_reduced_term_coefficient(ast_node *term);

coefficient_node *search_x_exponent_node(coefficient_node *coefficients,
		int x_exponent);
coefficient_node *search_max_x_exponent_node(coefficient_node *coefficients);

int coefficient_node_push_back(coefficient_node **coefficients, int x_exponent, double value);
void free_coefficients(coefficient_node *coefficients);

void coefficient_node_remove_node(coefficient_node **coefficients, coefficient_node *node);
void coefficient_node_remove_x_eponent_node(coefficient_node **coefficients,
		int x_exponent);

ast_node *coefficient_node_to_ast_term(coefficient_node *node);

#endif
