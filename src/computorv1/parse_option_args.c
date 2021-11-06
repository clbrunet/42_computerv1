#include <stdio.h>
#include <string.h>
#include "computorv1/options.h"
#include "computorv1/style.h"

static int parse_option_arg(char *arg, options *options)
{
	if (*arg != '-') {
		printf(BOLD FG_RED "error:" RESET_ALL " too many equation arguments\n"
				"\nFor more information try --help\n");
		return -1;
	}
	arg++;
	if (*arg != '-') {
		while (*arg != '\0') {
			if (*arg == 'h') {
				options->help = true;
			} else if (*arg == 't') {
				options->tree = true;
			} else {
				printf(BOLD FG_RED "error:" RESET_ALL " invalid argument '-%c'\n"
						"\nFor more information try --help\n", *arg);
				return -1;
			}
			arg++;
		}
	} else {
		arg++;
		if (strcmp(arg, "help") == 0) {
			options->help = true;
		} else if (strcmp(arg, "tree") == 0) {
			options->tree = true;
		} else {
			printf(BOLD FG_RED "error:" RESET_ALL " invalid argument '--%s'\n"
					"\nFor more information try --help\n", arg);
			return -1;
		}
	}
	return 0;
}

int parse_option_args(int argc, char *argv[], options *options)
{
	for (int i = 1; i < argc - 1; i++) {
		if (parse_option_arg(argv[i], options) == -1) {
			return -1;
		}
	}
	return 0;
}
