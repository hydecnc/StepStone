#!/bin/bash

set -ex

echo "running deploy-linux.sh"

if [ $# -ne 3 ]; then
  echo "Usage ./deploy-linux.sh linux_llm_path commit compiler"
  exit 1
fi

LINUX=$1
COMMIT=$2
COMPILER=$3
N_CORES=`nproc`

cd $LINUX

if [ ! -f "vmlinux" ]; then
    make clean
    git stash
    make olddefconfig CC=$COMPILER
    make -j$N_CORES CC=$COMPILER > make.log 2>&1
fi 

# save the dry run log
git checkout -f $COMMIT || (git pull https://github.com/torvalds/linux.git master > /dev/null 2>&1 && git checkout -f $COMMIT)
make olddefconfig CC=$COMPILER
find -type f -name '*.o' -delete
make -n CC=$COMPILER > dry_run_log 2>&1 || echo "It's OK"