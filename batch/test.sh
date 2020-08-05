#!/bin/bash

build_dir=/home/mikhail/hades_flow/cmake-build-debug

rm correction_in.root

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/mikhail/AnalysisTree/install/lib

echo "executing $build_dir/correct ${1}"
$build_dir/correct ${1}
mv correction_out.root correction_in.root

echo "executing $build_dir/correct ${1}"
$build_dir/correct ${1}
mv correction_out.root correction_in.root

echo "executing $build_dir/correct ${1} "
$build_dir/correct ${1}
mv correction_out.root correction_in.root

echo "executing $build_dir/correlate correction_out.root"
$build_dir/correlate correction_in.root

echo JOB FINISHED!