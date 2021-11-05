#include <stdio.h>
#include <string.h>
#include "computerv1/main.h"
#include "computerv1/style.h"
#include "computerv1/strings.h"
#include "computerv1/ast.h"

ast_node *ast_node_new(token token, double value, ast_node *left, ast_node *right)
{
	ast_node *new = xmalloc(sizeof(ast_node));
	if (new == NULL) {
		return (NULL);
	}
	new->token = token;
	new->value = value;
	new->left = left;
	new->right = right;
	return new;
}

ast_node *ast_node_dup(ast_node *node)
{
	ast_node *dup = ast_node_new(node->token, node->value, NULL, NULL);
	if (dup == NULL) {
		return (NULL);
	}
	if (node->left) {
		dup->left = ast_node_dup(node->left);
		if (dup->left == NULL) {
			free_ast(dup);
			return (NULL);
		}
	}
	if (node->right) {
		dup->right = ast_node_dup(node->right);
		if (dup->right == NULL) {
			free_ast(dup);
			return (NULL);
		}
	}
	return dup;
}

ast_node *ast_node_cpy(ast_node *dest, ast_node *src)
{
	dest->token = src->token;
	dest->value = src->value;
	if (src->left) {
		if (dest->left) {
			if (ast_node_cpy(dest->left, src->left) == NULL) {
				return NULL;
			}
		} else {
			dest->left = ast_node_dup(src->left);
			if (dest->left == NULL) {
				return NULL;
			}
		}
	} else {
		free_ast(dest->left);
		dest->left = NULL;
	}
	if (src->right) {
		if (dest->right) {
			ast_node_cpy(dest->right, src->right);
			if (ast_node_cpy(dest->right, src->right) == NULL) {
				return NULL;
			}
		} else {
			dest->right = ast_node_dup(src->right);
			if (dest->right == NULL) {
				return NULL;
			}
		}
	} else {
		free_ast(dest->right);
		dest->right = NULL;
	}
	return dest;
}

void set_ast_node(ast_node *node, token token, double value,
		ast_node *left, ast_node *right)
{
	node->token = token;
	node->value = value;
	node->left = left;
	node->right = right;
}

int token_precedence_compare(token t1, token t2)
{
	if (t1 == EQUALITY) {
		if (t2 == EQUALITY) {
			return 0;
		}
		return -1;
	} else if (t1 == ADDITION || t1 == SUBSTRACTION) {
		if (t2 == ADDITION || t2 == SUBSTRACTION) {
			return 0;
		} else if (t2 == EQUALITY) {
			return 1;
		}
		return -1;
	} else if (t1 == MULTIPLICATION || t1 == DIVISION) {
		if (t2 == MULTIPLICATION || t2 == DIVISION) {
			return 0;
		} else if (t2 == EQUALITY || t2 == ADDITION || t2 == SUBSTRACTION) {
			return 1;
		}
		return -1;
	} else if (t1 == EXPONENT) {
		if (t2 == EXPONENT) {
			return 0;
		} else if (t2 == VARIABLE || t2 == NUMBER) {
			return -1;
		}
		return 1;
	} else if (t1 == VARIABLE || t1 == NUMBER) {
		if (t2 == VARIABLE || t2 == NUMBER) {
			return 0;
		}
		return 1;
	}
	return t1 - t2;
}

static void print_ast_depth(ast_node *node, int depth, char id)
{
	if (node == NULL) {
		return;
	}
	printf(FG_DGRAY);
	for (int i = 0; i < depth; i++) {
		printf("▸ ");
	}
	printf(FG_LGRAY);
	printf("%c ", id);
	printf(RESET_ALL);
	switch (node->token) {
		case EQUALITY:
			printf("=\n");
			break;
		case ADDITION:
			printf("+\n");
			break;
		case SUBSTRACTION:
			printf("-\n");
			break;
		case MULTIPLICATION:
			printf("*\n");
			break;
		case DIVISION:
			printf("/\n");
			break;
		case VARIABLE:
			printf("X\n");
			break;
		case NUMBER:
			printf("%f\n", node->value);
			break;
		case EXPONENT:
			printf("^\n");
			break;
		default:
			printf("token : %d\n", node->token);
			break;
	}
	print_ast_depth(node->left, depth + 1, 'l');
	print_ast_depth(node->right, depth + 1, 'r');
}

void print_ast(ast_node *node)
{
	print_ast_depth(node, 0, 'R');
}

void free_ast(ast_node *node)
{
	if (node == NULL) {
		return;
	}
	free_ast(node->left);
	free_ast(node->right);
	free(node);
}

void free_ast_childs(ast_node *ast)
{
	free_ast(ast->left);
	ast->left = NULL;
	free_ast(ast->right);
	ast->right = NULL;
}

static char *node_token_to_string(ast_node *node)
{
	switch (node->token) {
		case EQUALITY:
			return strdup(" = ");
		case ADDITION:
			return strdup(" + ");
		case SUBSTRACTION:
			return strdup(" - ");
		case MULTIPLICATION:
			return strdup(" * ");
		case DIVISION:
			return strdup(" / ");
		case VARIABLE:
			return strdup("X");
		case NUMBER:
			return dtoa(node->value);
		case EXPONENT:
			return strdup("^");
		default:
			return NULL;
	}
}

char *ast_to_string(ast_node *node, ast_node *parent)
{
	if (node == NULL) {
		return NULL;
	}
	char *string = node_token_to_string(node);
	if (string == NULL) {
		return NULL;
	}
	char *joint = NULL;
	if (node->left != NULL) {
		char *left = ast_to_string(node->left, node);
		if (left == NULL) {
			free(string);
			return NULL;
		}
		joint = strjoin(left, string);
		free(left);
		free(string);
		if (joint == NULL) {
			return NULL;
		}
		string = joint;
	}
	if (node->right != NULL) {
		char *right = ast_to_string(node->right, node);
		if (right == NULL) {
			free(string);
			return NULL;
		}
		joint = strjoin(string, right);
		free(string);
		free(right);
		if (joint == NULL) {
			return NULL;
		}
		string = joint;
	}
	if (parent && parent->token != EQUALITY
		&& node->token != VARIABLE && node->token != NUMBER
		&& (token_precedence_compare(node->token, parent->token) < 0
			|| (node == parent->right
				&& token_precedence_compare(node->token, parent->token) == 0))) {
		joint = strsurround("(", string, ")");
		free(string);
		if (joint == NULL) {
			return NULL;
		}
		string = joint;
	}
	return string;
}
