#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "computorv1/main.h"

char *strrmv(char *s, int c)
{
	char *start = s;
	char *iterator = s;
	while (*iterator != '\0') {
		if (*iterator != c) {
			*s = *iterator;
			s++;
		}
		iterator++;
	}
	*s = '\0';
	return start;
}

char *strfind_first_not_of(char *s, const char *ignore)
{
	while (*s != '\0') {
		if (strchr(ignore, *s) == NULL) {
			return s;
		}
		s++;
	}
	return NULL;
}

char *strjoin(char *left, char *right)
{
	char *joint = malloc((strlen(left) + strlen(right) + 1) * sizeof(char));
	if (joint == NULL) {
		return NULL;
	}
	strcpy(joint, left);
	strcat(joint, right);
	return joint;
}

char *strsurround(char *left, char *middle, char *right)
{
	char *joint = malloc((strlen(left) + strlen(middle) + strlen(right) + 1)
			* sizeof(char));
	if (joint == NULL) {
		return NULL;
	}
	strcpy(joint, left);
	strcat(joint, middle);
	strcat(joint, right);
	return joint;
}

char *dtoa(double n)
{
	int length = snprintf(NULL, 0, "%f", n);
	char *a = malloc((length + 1)
			* sizeof(char));
	if (a == NULL) {
		return NULL;
	}
	sprintf(a, "%f", n);
	while (a[length - 1] == '0') {
		a[length - 1] = '\0';
		length--;
	}
	if (a[length - 1] == '.') {
		a[length - 1] = '\0';
	}
	return a;
}
