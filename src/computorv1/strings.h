#ifndef COMPUTORV1_STRINGS_H
#define COMPUTORV1_STRINGS_H

char *strrmv(char *s, int c);
char *strfind_first_not_of(char *s, const char *ignore);
char *strjoin(char *left, char *right);
char *strsurround(char *left, char *middle, char *right);
char *dtoa(double n);

#endif
