#!/bin/bash

###########################################################
# 1. cd project root dir
# 2. input stack info to stack.txt
# 3. ./tools/addr2line.sh
###########################################################

CURRENT_DIR=$(cd $(dirname $0); pwd)
PROJECT_DIR=$(cd $(dirname $CURRENT_DIR); pwd)
ADDR2LINE=$PROJECT_DIR/src/third_party/android_ndk/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-addr2line
UNSTRIP_SO_DIR=$PROJECT_DIR/src/out/Chromium/lib.unstripped
STACK_FILE=$PROJECT_DIR/stack.txt

REGEX_STRING="pc (.+?) .*/(.*.so)"

unstack_ne_address() {
  line=${1}
  typeset ec
  [[ $line =~ $REGEX_STRING ]]
  ec=$?
  if [ ! $ec -eq 0 ]
  then
    return
  fi

  if [ -n $BASH_VERSION ]
  then
    addr=${BASH_REMATCH[1]}
    so=${BASH_REMATCH[2]}

    if [ $so == 'libchrome.so' ]
    then
      $ADDR2LINE -C -e $UNSTRIP_SO_DIR/$so $addr
    else
      echo "unknown $so $addr"
    fi
  elif [ -n $KSH_VERSION ]
  then
    echo ${.sh.match[0]} ${.sh.match[1]} ${.sh.match[2]}
  elif [ -n $ZSH_VERSION ]
  then
    echo ${match[0]} ${match[1]} ${match[2]}
  fi
}

unstack_from_target_file() {
  while read line
  do
    unstack_ne_address "$line"
  done < $STACK_FILE
}

unstack_from_target_file
