#!/usr/bin/env bash

# *******************************************************************************
# Copyright (c) 2019 UT-Battelle, LLC.
# All rights reserved. This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v1.0
# and Eclipse Distribution License v.10 which accompany this distribution.
# The Eclipse Public License is available at http://www.eclipse.org/legal/epl-v10.html
# and the Eclipse Distribution License is available at
# https://eclipse.org/org/documents/edl-v10.php
#
# Contributors:
#   Alexander J. McCaskey - initial API and implementation
# *******************************************************************************/

clang_format_executable=${CLANG_FORMAT_EXE:-clang-format}

this_program=$(basename "$0")
usage="Usage:
  $this_program [options] -- check formatting of the C++ source files

Options:
    -h --help           Print help and exit
    -q --quiet          Quiet mode (do not print the format difference)
    -p --apply-patch    Apply format patch to the source files"

verbose=1
apply_patch=0

while [ $# -gt 0 ]
do
  case $1 in
  -p|--apply-patch)
      apply_patch=1
      ;;
  -q|--quiet)
      verbose=0
      ;;
  -h|--help)
      echo "$usage"
      exit 0
      ;;
  *)
      echo "$this_program: Unknown argument '$1'. See '$this_program --help'."
      exit -1
      ;;
  esac

  shift
done

# Redirect everything to /dev/null in quiet mode
if [ $verbose -eq 0 ]; then
    exec &>/dev/null
fi

cpp_source_files=$(git ls-files xacc quantum python | grep -E ".hpp$|.cpp$|.h$|.c$" | grep -v -f .clang-format-ignore)

unformatted_files=()
for file in $cpp_source_files; do
    diff -u \
         <(cat $file) \
         --label a/$file \
         <($clang_format_executable $file) \
         --label b/$file >&1
    if [ $? -eq 1 ]; then
        unformatted_files+=($file)
    fi
done

n_unformatted_files=${#unformatted_files[@]}
if [ $n_unformatted_files -ne 0 ]; then
    echo "${#unformatted_files[@]} file(s) not formatted properly:"
    for file in ${unformatted_files[@]}; do
        echo "  $file"
        if [ $apply_patch -eq 1 ]; then
            $clang_format_executable -i $file
            ((n_unformatted_files--))
        fi
    done
else
   echo "OK"
fi
echo $n_unformatted_files
exit $n_unformatted_files
