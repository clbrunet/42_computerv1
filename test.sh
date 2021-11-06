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

computor()
{
  echo "================================================================================"
  echo -e "↓ $MAGENTA'$1'$RESET ↓"
  ./computor "$1"
  echo -e "↑ $MAGENTA'$1'$RESET ↑"
}

echo -e "${BOLD}Equation format errors :$RESET"
computor "1 + -q = 0"
computor "="
computor "= 0"
computor "0 ="
computor "1 + = 0"
computor "+ 1 = 0"
computor "1 * = 0"
computor "* 1 = 0"
computor "1 ^ = 0"
computor "^ 1 = 0"
computor "1 + ( = 0"
computor "99999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999 = 0"
computor "-- = 0"
computor "1^X = 0"
computor "1^8.5 = 0"
computor "1^-8 = 0"
computor "1 + 1"
computor "1 + 1 = 0 ="
computor "= 1 + 1 = 0"
echo

echo
echo -e "${BOLD}Subject equations :$RESET"
computor "5 * X^0 + 4 * X^1 - 9.3 * X^2 = 1 * X^0"
computor "5 * X^0 + 4 * X^1 = 4 * X^0"
computor "8 * X^0 - 6 * X^1 + 0 * X^2 - 5.6 * X^3 = 3 * X^0"
computor "42 * X^0 = 42 * X^0"
computor "5 + 4 * X + X^2= X^2"
echo

echo
echo -e "${BOLD}0 degree equations :$RESET"
computor "42 = 42"
computor "0 = 42"
computor "0 - 42 * X^0 = 42"
echo

echo
echo -e "${BOLD}1 degree equations :$RESET"
computor "X = 42"
computor "42 = X"
computor "X^1 = 42 * X^0"
computor "2X - 2 = 0"
echo

echo
echo -e "${BOLD}2 degree equations :$RESET"
computor "X^2 = 42"
computor "42 = X^2"
computor "X^2 + X = 42"
computor "42 = X^2 + X"
computor "42 * X^2 + 42 * X^1 + 42 = 0"
computor "-42 * X^2 - 42 * X^1 - 42 = 0"
computor "-42 * X^2 = 0"
computor "X^2 + 4X + 4 = 0"
