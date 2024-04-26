#!/bin/bash
#SBATCH --ntasks=1
#SBATCH --time=01:00:00
#SBATCH --job-name asdfio
#SBATCH --output=out.log
#SBATCH --partition Optane

module load fftw
export LD_LIBRARY_PATH=/home/bxh220006/scratch/local_attribute:$LD_LIBRARY_PATH

source ~/software/mada/share/madagascar/etc/env.sh
echo "=================================================="

data_lst=./data/preproc.lst
num_dates=`cat $data_lst |wc -l`
stat_lst=./data/station.txt
num_dates=`cat $data_lst |wc -l`
num_stats=`cat $stat_lst |wc -l`
num_stats=`echo $num_stats | awk '{print $1-1}'`
num_stats_minus_one=`echo $num_stats | awk '{print $1-1}'`
echo 'num of stations: ' $num_stats_minus_one 'will be stacked for num of dates: '$num_dates 
num_dates_for_sum=(5 15 30)

:>master_station.lst
for istam in `seq 1 1`;do
  sta_in_file=`echo $istam |awk '{print $1+1}'`
  line=`sed "${sta_in_file}q;d" $stat_lst | awk '{print $1}'`
  stnw=`echo $line |awk -F"," '{print $1}'`
  stnm=`echo $line |awk -F"," '{print $2}'`
  stcm=`echo $line |awk -F"," '{print $3}'`
  comp=`echo ${stcm:2:3}`
  evla=`echo $line |awk -F"," '{print $4}'`
  evlo=`echo $line |awk -F"," '{print $5}'`
  master_station_name=`echo $stnw $stnm $stcm |awk '{print $1"."$2"."$3}'`
  master_station_dir=`echo $stnw $stnm $comp | awk '{print $1"."$2"_"$3$3}'`
  echo $master_station_name >> master_station.lst
  mkdir -p data/$master_station_dir
  output_path=data/$master_station_dir
  :>convergent_sstacking.lst
  for iallday in ${num_dates_for_sum[@]};do
    for istas in `seq 2 $num_stats`;do
      sta_in_file=`echo $istas |awk '{print $1+1}'`
      line=`sed "${sta_in_file}q;d" $stat_lst | awk '{print $1}'`
      stnw=`echo $line |awk -F"," '{print $1}'`
      stnm=`echo $line |awk -F"," '{print $2}'`
      stcm=`echo $line |awk -F"," '{print $3}'`
      stla=`echo $line |awk -F"," '{print $4}'`
      stlo=`echo $line |awk -F"," '{print $5}'`
      slaver_station_name=`echo $stnw $stnm $stcm |awk '{print $1"."$2"."$3}'`
      station_pair=`echo $master_station_name $slaver_station_name |awk '{print $1"_"$2}'`
      :>sstack_for_sum.lst
      for iday in `seq $iallday`;do
        day=`echo $iday | awk '{printf"day%05d",$1}'`
        ls data/$day/sstack_$station_pair.rsf >>sstack_for_sum.lst
      done
      num=`cat sstack_for_sum.lst | wc -l`
      
      sstack_out=$output_path/${iallday}_sstack_$station_pair.rsf
      
      ./myadd ccfile=sstack_for_sum.lst >$sstack_out  --out=stdout

      dist=` ~/software/sactools_c/distbaz $evlo $evla $stlo $stla | awk '{print $1}'`
      refvs=2.8
      reftime=`echo $dist $refvs | awk '{print $1/$2}'`
      
      delta=`cat Download/Preproc/fft_cc_data.txt | awk -F"," '{print $2}' |awk -F":" '{print $2}'`
      sh rsf2sac.bash $sstack_out -120 $delta $stla $stlo $evla $evlo
    done
    echo  "$iallday $sstack_out.sac $reftime" >>convergent_sstacking.lst
  done
done
rm dummy_data* -rf
