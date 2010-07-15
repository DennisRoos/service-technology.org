#!/usr/bin/env bash

for param in "$@"
do
  if [ "$param" = "--help" ];
  then
    echo "Usage: publicview.sh NET"
    exit
  fi
  if [ "$param" = "--version" ];
  then
    echo "Sayo 1.2-unreleased"
    exit
  fi
done


NET=$1
WENDY=wendy
SA2SM=sa2sm
SAYO=sayo
FORMULA2BITS=wendyFormula2bits

${WENDY} --og=${NET}.formula.og ${NET} &> /dev/null

${FORMULA2BITS} -i ${NET}.formula.og -o ${NET}.og

${SAYO} -i ${NET}.og -o ${NET}.sa

${SA2SM} ${NET}.sa ${NET}.pv.owfn

rm ${NET}.og ${NET}.sa ${NET}.formula.og
