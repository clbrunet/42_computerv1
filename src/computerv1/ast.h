#ifndef COMPUTERV1_AST_H
#define COMPUTERV1_AST_H

#include <stdlib.h>

typedef enum token {
	EQUALITY,
	ADDITION,
	SUBSTRACTION,
	MULTIPLICATION,
	DIVISION,
	VARIABLE,
	NUMBER,
	EXPONENT,
} token;

typedef struct ast_node
{
	token token;
	double value;
	struct ast_node *left;
	struct ast_node *right;
} ast_node;

ast_node *ast_node_new(token token, double value, ast_node *left, ast_node *right);
void set_ast_node(ast_node *node, token token, double value,
		ast_node *left, ast_node *right);
int token_precedence_compare(token t1, token t2);
void print_ast(ast_node *node);
void free_ast(ast_node *node);
char *ast_to_string(ast_node *node, ast_node *parent);

#endif
