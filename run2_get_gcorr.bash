#!/bin/bash
#SBATCH --ntasks=31
#SBATCH --time=48:00:00
#SBATCH --job-name ccf
#SBATCH --output=out2g.log
#SBATCH --partition Optane-long

nproc=31
export LD_LIBRARY_PATH=/home/bxh220006/scratch/local_attribute:$LD_LIBRARY_PATH
echo "=================================================="
echo "gather all the data for ccf"
alldata_lst=./data/alldata_for_gccf.lst
data_lst=./data/preproc.lst
ccf_lst=public_ccf.lst
num_dates=`cat $data_lst |wc -l`

nccfs=5760
taumax=120
gcorr_len=3600
gcorr_overlap=2700
num_ccfs=`cat $ccf_lst | wc -l`
echo 'num of dates: '$num_dates 'with num of ccfs: ' $num_ccfs
start=`date +%s.%N`
:>$alldata_lst
for iday in `seq 1 $num_dates`;do
    noise_day_in_path=`sed "${iday}q;d" $data_lst`
    noise_day_in_path=`echo $iday | awk '{printf"./data/day%05d",$1}'`
    
    for iccf in `seq 1 $num_ccfs`;do
        station_pair=`sed "${iccf}q;d" $ccf_lst | awk '{print $1}'`
        master_station_name=`echo $station_pair | awk -F"_" '{print $1}'`
        slaver_station_name=`echo $station_pair | awk -F"_" '{print $2}'`
        ccf_infile=${noise_day_in_path}/$master_station_name.rsf
        ccf_other=${noise_day_in_path}/$slaver_station_name.rsf
        gccf_outfile=${noise_day_in_path}/gccf_$station_pair.rsf
        echo $ccf_infile $ccf_other $gccf_outfile $gcorr_len $gcorr_overlap $nccfs $taumax>>$alldata_lst
    done
done
mpirun -np $nproc ./subrun_gcorr.bash 

end=`date +%s.%N`
runtime=$( echo "$end - $start" | bc -l )
echo 'glob stacking running: '$runtime

