#!/bin/bash
#SBATCH --ntasks=31
#SBATCH --time=01:00:00
#SBATCH --job-name ccf
#SBATCH --output=out3.log
#SBATCH --partition Optane

nproc=31
export LD_LIBRARY_PATH=/home/bxh220006/scratch/local_attribute:$LD_LIBRARY_PATH

echo "=================================================="
echo "stacking the gccf for reference"
ccf_lst=public_ccf.lst
num_ccfs=`cat $ccf_lst | wc -l`
data_lst=./data/preproc.lst
num_dates=`cat $data_lst |wc -l`
cat $ccf_lst | awk -F"_" '{print $1}' | uniq >dummy_ccf.lst
for master_station_dir in `cat dummy_ccf.lst`;do
  rm -rf data/$master_station_dir
  mkdir -p data/$master_station_dir
done
rm dummy_ccf.lst

cat $ccf_lst | awk '{print $1}' >data/subadd.lst
mpirun -np $nproc ./subrun_add.bash



