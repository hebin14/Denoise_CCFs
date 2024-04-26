#!/bin/bash
#SBATCH --ntasks=31
#SBATCH --time=03:00:00
#SBATCH --job-name asdfio
#SBATCH --output=out5lg.log
#SBATCH --partition Optane

nproc=31
module load fftw
export LD_LIBRARY_PATH=/home/bxh220006/scratch/local_attribute:$LD_LIBRARY_PATH
echo "=================================================="
alldata_lst=./data/alldata_for_lstack.lst
data_lst=./data/preproc.lst
stat_lst=./data/station.txt
num_dates=`cat $data_lst |wc -l`
num_stats=`cat $stat_lst |wc -l`
echo 'num of dates: '$num_dates
start=`date +%s.%N`
:>$alldata_lst
for iday in `seq 1 $num_dates`;do
    echo "..................................................iday="$iday
    noise_day_in_path=`sed "${iday}q;d" $data_lst`
    noise_day_in_path=`echo $iday | awk '{printf"./data/day%05d",$1}'`
    ccf_lst=public_ccf.lst
    num_ccfs=`cat $ccf_lst | wc -l`
    
    
    for iccf in `seq 1 $num_ccfs`;do
      station_pair_name=`sed "${iccf}q;d" $ccf_lst | awk '{print $1}'`
      dist=`sed "${iccf}q;d" $ccf_lst | awk '{print $2}'`
      lccf_infile=${noise_day_in_path}/lccf_$station_pair_name.rsf
      gccf_infile=${noise_day_in_path}/gccf_$station_pair_name.rsf
      simi_infile=${noise_day_in_path}/simi_$station_pair_name.rsf
      thld_outfile=${noise_day_in_path}/thld_$station_pair_name.rsf          
      gstack_outfile=${noise_day_in_path}/gstack_$station_pair_name.rsf
      lstack_outfile=${noise_day_in_path}/lstack_$station_pair_name.rsf
      echo $lccf_infile $gccf_infile $simi_infile $thld_outfile $lstack_outfile $gstack_outfile>>$alldata_lst

    done
   
done
 mpirun -np $nproc ./subrun_lstack.bash
end=`date +%s.%N`
runtime=$( echo "$end - $start" | bc -l )
echo 'rms stacking running: '$runtime
