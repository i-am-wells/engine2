#!/bin/bash
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
OUTDIR=$1
cd $DIR/OpenBLAS
make && cp $(readlink libopenblas.a) $OUTDIR/libopenblas_engine2.a
