#!/bin/bash
#SBATCH --ntasks=31
#SBATCH --time=01:00:00
#SBATCH --job-name ccf
#SBATCH --output=out.log
#SBATCH --partition Optane-long

nproc=31
export LD_LIBRARY_PATH=/home/bxh220006/scratch/local_attribute:$LD_LIBRARY_PATH
echo "=================================================="
echo "gather all the data for ccf"
alldata_lst=./data/alldata_for_sstack.lst
data_lst=./data/preproc.lst
stat_lst=./data/station.txt
num_dates=`cat $data_lst |wc -l`
num_stats=`cat $stat_lst |wc -l`
fold=0
taumax=120
vsmin=2.8
vsmax=2.9
periodmax=8
gcorr_len=3600
gcorr_overlap=2700
echo 'num of dates: '$num_dates 'with num of stations: ' $num_stats
epsilon=`echo $num_dates | awk '{print 1+1.0/($1*24)}'`

ccf_lst=public_ccf.lst
num_ccfs=`cat $ccf_lst | wc -l`
echo 'num of dates: '$num_dates 'with num of ccfs: ' $num_ccfs
start=`date +%s.%N`
:>$alldata_lst
for iday in `seq 1 $num_dates`;do
    noise_day_in_path=`sed "${iday}q;d" $data_lst`
    noise_day_in_path=`echo $iday | awk '{printf"./data/day%05d",$1}'`
   
    if [ -d "$noise_day_in_path" ]; then
        for iccf in `seq 1 $num_ccfs`;do
          station_pair=`sed "${iccf}q;d" $ccf_lst | awk '{print $1}'`
          dist=`sed "${iccf}q;d" $ccf_lst | awk '{print $2}'`
          master_station_name=`echo $station_pair | awk -F"_" '{print $1}'`
          slaver_station_name=`echo $station_pair | awk -F"_" '{print $2}'`
          ccf_infile=${noise_day_in_path}/$master_station_name.rsf
          ccf_other=${noise_day_in_path}/$slaver_station_name.rsf
          stacked=data/${master_station_name}/refgccf_$station_pair.rsf
          gccf_outfile=${noise_day_in_path}/sstack_$station_pair.rsf
          if [ -f "$ccf_infile" ] && [ -f "$ccf_other" ] && [ -f "$stacked" ];then
            echo $ccf_infile $ccf_other $stacked $gccf_outfile $dist $num_dates $fold $taumax $vsmin $vsmax $periodmax $gcorr_len $gcorr_overlap>>$alldata_lst
          fi
        done
    fi
done
mpirun -np $nproc ./subrun_rms_stack.bash
end=`date +%s.%N`
runtime=$( echo "$end - $start" | bc -l )
echo 'rms stacking running: '$runtime
echo $epsilon '==================================================='

