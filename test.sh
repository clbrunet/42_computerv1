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

launch()
{
  echo "====================================="
  echo -e "↓ $MAGENTA'$1'$RESET ↓"
  ./computer "$1"
  echo -e "↑ $MAGENTA'$1'$RESET ↑"
}

echo -e "${BOLD}Equation format errors :$RESET"
launch "1 + -q = 0"
launch "="
launch "= 0"
launch "0 ="
launch "1 + = 0"
launch "+ 1 = 0"
launch "1 * = 0"
launch "* 1 = 0"
launch "1 ^ = 0"
launch "^ 1 = 0"
launch "1 + ( = 0"
launch "99999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999 = 0"
launch "-- = 0"
launch "1^X = 0"
launch "1^8.5 = 0"
launch "1 + 1"
launch "1 + 1 = 0 ="
