#!/bin/sh
sort -h $1 > scenario1_student.txt
sort -h $2 > scenario1_profs.txt

if diff -q scenario1_student.txt scenario1_profs.txt >/dev/null; then
  echo "IGUAIS"
else
  echo "DIFERENTES"
fi
