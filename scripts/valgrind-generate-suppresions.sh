#!/bin/sh

VALGRIND_LOG=$(mktemp)
valgrind --leak-check=full --gen-suppressions=all --log-file=${VALGRIND_LOG}  ./ref-app/refopenglapp -noopengl  >/dev/null
cat ${VALGRIND_LOG}  | ./parse_valgrind_suppressions.sh > minimal.supp
rm ${VALGRIND_LOG}

