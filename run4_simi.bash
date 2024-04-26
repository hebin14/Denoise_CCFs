#!/bin/bash
#SBATCH --ntasks=31
#SBATCH --time=48:00:00
#SBATCH --job-name ccf
#SBATCH --output=out4.log
#SBATCH --partition Optane-long

nproc=31
export LD_LIBRARY_PATH=/home/bxh220006/scratch/noise_snr/234A_Z36A_one_day:$LD_LIBRARY_PATH

echo "=================================================="
echo "gather all the data for similarity processing"
alldata_lst=./data/alldata_for_similarity.lst
data_lst=./data/preproc.lst
num_dates=`cat $data_lst |wc -l`
echo 'num of dates: '$num_dates
start=`date +%s.%N`
:>$alldata_lst
for iday in `seq 1 $num_dates`;do
    noise_day_in_path=`sed "${iday}q;d" $data_lst`
    noise_day_in_path=`echo $iday | awk '{printf"./data/day%05d",$1}'`
    
    stat_lst=public_ccf.lst
    num_stats=`cat $stat_lst |wc -l`
    echo 'collecting ccf for similarity and threshold in day: '$iday 'from ' $noise_day_in_path,'with num of station-pairs: '$num_stats
    for ista in `seq 1 $num_stats`;do
      station_pair_name=`sed "${ista}q;d" $stat_lst | awk '{print $1}'`
      master_station_dir=`echo $station_pair_name | awk -F"_" '{print "data/"$1}'`
      lccf_infile=${noise_day_in_path}/lccf_$station_pair_name.rsf
      gccf_infile=${master_station_dir}/refgccf_$station_pair_name.rsf
      simi_outfile=${noise_day_in_path}/simi_$station_pair_name.rsf
          
      echo $lccf_infile $gccf_infile $simi_outfile >>$alldata_lst
    done
    
done
mpirun -np $nproc ./subrun_simi.bash
end=`date +%s.%N`
runtime=$( echo "$end - $start" | bc -l )
echo 'glob stacking running: '$runtime



