#!/bin/bash
#SBATCH --ntasks=1
#SBATCH --time=01:00:00
#SBATCH --job-name asdfio
#SBATCH --output=out.log
#SBATCH --partition Optane

module load fftw
export LD_LIBRARY_PATH=/home/bxh220006/scratch/noise_snr/234A_Z36A_one_month_two:$LD_LIBRARY_PATH

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


for iallday in `seq 1 $num_dates`;do
  mkdir -p dummy_data_$iallday
  for iday in `seq 1 $iallday`;do
      date0=`echo $iday | awk '{printf"./data/day%05d",$1}'`
      date1=`echo $iday $iallday | awk '{printf"./dummy_data_%d/day%05d",$2,$1}'`
      cp $date0 $date1 -r
  done
done

:>master_station.lst
for istam in `seq 1 $num_stats_minus_one`;do
  sta_in_file=`echo $istam |awk '{print $1+1}'`
  line=`sed "${sta_in_file}q;d" $stat_lst | awk '{print $1}'`
  stnw=`echo $line |awk -F"," '{print $1}'`
  stnm=`echo $line |awk -F"," '{print $2}'`
  stcm=`echo $line |awk -F"," '{print $3}'`
  comp=`echo ${stcm:2:3}`
  master_station_name=`echo $stnw $stnm $stcm |awk '{print $1"."$2"."$3}'`
  master_station_dir=`echo $stnw $stnm $comp | awk '{print $1"."$2"_"$3$3}'`
  echo $master_station_name >> master_station.lst
  if [ -d "data/$master_station_dir" ]; then
    output_path=data/$master_station_dir
    :>convergent_lstacking_snr.lst
    :>convergent_gstacking_snr.lst
  
  for istas in `seq 2 $num_stats`;do
    sta_in_file=`echo $istas |awk '{print $1+1}'`
    line=`sed "${sta_in_file}q;d" $stat_lst | awk '{print $1}'`
    stnw=`echo $line |awk -F"," '{print $1}'`
    stnm=`echo $line |awk -F"," '{print $2}'`
    stcm=`echo $line |awk -F"," '{print $3}'`
    slaver_station_name=`echo $stnw $stnm $stcm |awk '{print $1"."$2"."$3}'`
    station_pair=`echo $master_station_name $slaver_station_name |awk '{print $1"_"$2}'`
    
    all_gstack_out=$output_path/gstack_$station_pair.rsf
    all_gstack_for_sum=`ls data/day000*/gstack_$station_pair.rsf`
      
    scale=`echo $num_dates | awk '{print 1.0/$1}'`
    echo "sfadd >$all_gstack_out <`echo $all_gstack_for_sum ` scale=$scale" >tmp.bash
    sh tmp.bash
    for iallday in `seq 1 $num_dates`;do
      echo 'iday:' $iallday 'for ith master staion:' $istam 
      gstack_for_sum=`ls dummy_data_$iallday/day000*/gstack_$station_pair.rsf`
      lstack_for_sum=`ls dummy_data_$iallday/day000*/lstack_$station_pair.rsf`
      gstack_out=$output_path/${iallday}_gstack_$station_pair.rsf
      lstack_out=$output_path/${iallday}_lstack_$station_pair.rsf
      scale=`echo $iallday | awk '{print 1.0/$1}'`
      echo "sfadd >$gstack_out <`echo $gstack_for_sum ` scale=$scale" >tmp.bash
      echo "sfadd >$lstack_out <`echo $lstack_for_sum ` scale=$scale" >>tmp.bash
      sh tmp.bash
      ./mysnr <$lstack_out other=$all_gstack_out sb=11 se=44 nb=44 ne=76 get_corr=y >tmp
      snr=`cat tmp | awk '{print $1}'`
      corr=`cat tmp | awk '{print $2}'`
      echo  "$iallday $snr $corr " >>convergent_lstacking_snr.lst
      ./mysnr <$gstack_out other=$all_gstack_out sb=sb=11 se=44 nb=44 ne=76 get_corr=y >tmp
      snr=`cat tmp | awk '{print $1}'`
      corr=`cat tmp | awk '{print $2}'`
      echo  "$iallday $snr $corr " >>convergent_gstacking_snr.lst
    done
  done
  fi
  exit
done
rm *dummy_data* -rf
