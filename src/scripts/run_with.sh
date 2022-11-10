#!/bin/sh

scribble=0x00
region_size=4096
cmd=$@

here=$(dirname -- "$( readlink -f -- "$0"; )" )

while getopts s:r:e: flag
do
  case "${flag}" in
    s) scribble=${OPTARG};;
    r) region_size=${OPTARG};;
    e) cmd=${OPTARG};;
  esac
done

echo "scribble ${scribble}; region_size ${region_size}; command: ${cmd}"

make

LD_PRELOAD=${here}/lynx_alloc_shared.so \
  MALLOC_SCRIBBLE=${scribble} \
  MALLOC_REGION_SIZE=${region_size} \
  ${cmd}
