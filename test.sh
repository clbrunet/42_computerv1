#!/bin/bash

RESET="\x1B[0m"
BOLD="\x1B[1m"
BLACK="\x1B[30m"
RED="\x1B[31m"
GREEN="\x1B[32m"
YELLOW="\x1B[33m"
BLUE="\x1B[34m"
MAGENTA="\x1B[35m"
CYAN="\x1B[36m"
WHITE="\x1B[37m"


make

computer()
{
  echo "================================================================================"
  echo -e "↓ $MAGENTA'$1'$RESET ↓"
  ./computer "$1"
  echo -e "↑ $MAGENTA'$1'$RESET ↑"
}

echo -e "${BOLD}Equation format errors :$RESET"
computer "1 + -q = 0"
computer "="
computer "= 0"
computer "0 ="
computer "1 + = 0"
computer "+ 1 = 0"
computer "1 * = 0"
computer "* 1 = 0"
computer "1 ^ = 0"
computer "^ 1 = 0"
computer "1 + ( = 0"
computer "99999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999 = 0"
computer "-- = 0"
computer "1^X = 0"
computer "1^8.5 = 0"
computer "1^-8 = 0"
computer "1 + 1"
computer "1 + 1 = 0 ="
computer "= 1 + 1 = 0"
echo

echo
echo -e "${BOLD}Subject equations :$RESET"
computer "5 * X^0 + 4 * X^1 - 9.3 * X^2 = 1 * X^0"
computer "5 * X^0 + 4 * X^1 = 4 * X^0"
computer "8 * X^0 - 6 * X^1 + 0 * X^2 - 5.6 * X^3 = 3 * X^0"
computer "5 + 4 * X + X^2= X^2"
