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
  :>convergent_gstacking.lst
  :>convergent_lstacking.lst
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
      :>gstack_for_sum.lst
      :>lstack_for_sum.lst
      for iday in `seq $iallday`;do
        day=`echo $iday | awk '{printf"day%05d",$1}'`
        ls data/$day/gstack_$station_pair.rsf >>gstack_for_sum.lst
        ls data/$day/lstack_$station_pair.rsf >>lstack_for_sum.lst
      done
      num=`cat gstack_for_sum.lst | wc -l`
      
      gstack_out=$output_path/${iallday}_gstack_$station_pair.rsf
      lstack_out=$output_path/${iallday}_lstack_$station_pair.rsf
      ./myadd ccfile=gstack_for_sum.lst >$gstack_out  --out=stdout
      ./myadd ccfile=lstack_for_sum.lst >$lstack_out  --out=stdout
      delta=`cat Download/Preproc/fft_cc_data.txt | awk -F"," '{print $2}' |awk -F":" '{print $2}'`
      sh rsf2sac.bash $gstack_out -120 $delta $stla $stlo $evla $evlo
      sh rsf2sac.bash $lstack_out -120 $delta $stla $stlo $evla $evlo
    done
    echo  "$iallday $gstack_out.sac " >>convergent_gstacking.lst
    echo  "$iallday $lstack_out.sac " >>convergent_lstacking.lst
  done
done
rm dummy_data* -rf
