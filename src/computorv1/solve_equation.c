#include <stdio.h>
#include "computorv1/strings.h"
#include "computorv1/utils.h"
#include "computorv1/ast.h"
#include "computorv1/coefficient_node.h"

void solve_0_polynomial_degree_equation(ast_node *ast)
{
	if (is_number_zero(ast->left->value)) {
		printf("All real numbers are solutions.\n");
	} else {
		printf("There are no solutions.\n");
	}
}

int solve_1_polynomial_degree_equation(ast_node *ast)
{
	coefficient_node *coefficients = get_expression_coefficients(ast->left);
	if (coefficients == NULL) {
		return -1;
	}
	double divisor = coefficients->value;
	double numerator = (coefficients->next == NULL) ? 0 : -coefficients->next->value;
	char *solution = dtoa(numerator / divisor);
	if (solution == NULL) {
		free_coefficients(coefficients);
		return -1;
	}
	printf("The solution is:\n%s\n", solution);
	free(solution);
	free_coefficients(coefficients);
	return 0;
}

int solve_2_polynomial_degree_equation(ast_node *ast)
{
	coefficient_node *coefficients = get_expression_coefficients(ast->left);
	if (coefficients == NULL) {
		return -1;
	}
	double a = coefficients->value;
	double b = 0;
	double c = 0;
	coefficient_node *iter = coefficients->next;
	while (iter) {
		switch (iter->x_exponent) {
			case 1:
				b = iter->value;
				break;
			case 0:
				c = iter->value;
				break;
		}
		iter = iter->next;
	}
	free_coefficients(coefficients);
	double discriminant = b * b - 4 * a * c;
	if (discriminant < 0) {
		char *solution_lhs = dtoa(-b / (2 * a));
		char *solution_rhs = dtoa(ft_sqrt(-discriminant) / (2 * a));
		if (solution_lhs == NULL || solution_rhs == NULL) {
			free(solution_lhs);
			free(solution_rhs);
			return -1;
		}
		printf("Discriminant is strictly negative, the two complex solutions are:\n"
				"%s - %si\n%s + %si\n", solution_lhs, solution_rhs, solution_lhs, solution_rhs);
		free(solution_lhs);
		free(solution_rhs);
	} else if (is_number_zero(discriminant)) {
		char *solution = dtoa(-b / (2 * a));
		if (solution == NULL) {
			return -1;
		}
		printf("Discriminant is zero, the solution is:\n%s\n", solution);
		free(solution);
	} else {
		char *solution1 = dtoa((-b - ft_sqrt(discriminant)) / (2 * a));
		char *solution2 = dtoa((-b + ft_sqrt(discriminant)) / (2 * a));
		if (solution1 == NULL || solution2 == NULL) {
			free(solution1);
			free(solution2);
			return -1;
		}
		printf("Discriminant is strictly positive, the two solutions are:\n%s\n%s\n",
				solution1, solution2);
		free(solution1);
		free(solution2);
	}
	return 0;
}
