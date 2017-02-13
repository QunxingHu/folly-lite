#!/bin/sh

prefix=/home/hqx/build/folly-lite/jemalloc
exec_prefix=/home/hqx/build/folly-lite/jemalloc
libdir=${exec_prefix}/lib

LD_PRELOAD=${libdir}/libjemalloc.so.2
export LD_PRELOAD
exec "$@"
