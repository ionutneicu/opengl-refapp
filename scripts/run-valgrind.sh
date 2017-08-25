#!/bin/bash


APP=./refopenglapp
SCRIPTS_PATH=$(dirname "$0")
echo "step 1/2 generating valgrind suppresions, please wait"
VALGRIND_LOG=$(mktemp)

rm -f $VALGRIND_LOG
valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all --gen-suppressions=all --log-file=${VALGRIND_LOG}  ${APP} --loops 1000  >/dev/null
cat ${VALGRIND_LOG} | ${SCRIPTS_PATH}/parse_valgrind_suppressions.sh > minimal.supp
rm ${VALGRIND_LOG}


echo "step 2/2 running valgrind, please wait..."
#valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all --suppressions=minimal.supp ${APP} --leak_textures -loops 10 > ${VALGRIND_LOG} 2>&1
valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all --suppressions=./minimal.supp ${APP} --loops 10000 > ${VALGRIND_LOG} 2>&1

cat ${VALGRIND_LOG}
