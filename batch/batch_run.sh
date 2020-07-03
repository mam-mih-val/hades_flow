#!/bin/bash

format='+%Y/%m/%d-%H:%M:%S'

date $format

job_num=$(($SLURM_ARRAY_TASK_ID))

filelist=$lists_dir/$job_num.list

cd $output_dir
mkdir -p $job_num
cd $job_num

while read line; do
    echo $line >> list.txt
done < $filelist1
echo >> list.txt

source /etc/profile.d/modules.sh
module use /cvmfs/it.gsi.de/modulefiles/
module load compiler/gcc/9

echo "loading " $ownroot
source $ownroot

echo "executing $build_dir/correct list.txt"
$build_dir/correct list.txt
mv correction_out.root correction_in.root

echo "executing $build_dir/correct list.txt"
$build_dir/correct list.txt
mv correction_out.root correction_in.root

echo "executing $build_dir/correct list.txt"
$build_dir/correct list.txt
mv correction_out.root correction_in.root

echo "executing $build_dir/correlate correction_out.root"
$build_dir/correlate correction_out.root

echo JOB FINISHED!