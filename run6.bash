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
echo 'num of stations: ' $num_stats 'will be stacked for num of dates: '$num_dates 
:>master_station.lst
for istam in `seq 1 $num_stats_minus_one`;do
      sta_in_file=`echo $istam |awk '{print $1+1}'`
      line=`sed "${sta_in_file}q;d" $stat_lst`
      stnw=`echo $line |awk -F"," '{print $1}'`
      stnm=`echo $line |awk -F"," '{print $2}'`
      stcm=`echo $line |awk -F"," '{print $3}'`
      comp=`echo ${stcm:2:3}`
      master_station_name=`echo $stnw $stnm $stcm |awk '{print $1"."$2"."$3}'`
      master_station_dir=`echo $stnw $stnm $comp | awk '{print $1"."$2"_"$3$3}'`
      echo $master_station_name >> master_station.lst
      if [  -d "data/$master_station_dir" ]; then
        rm -rf data/$master_station_dir
        mkdir data/$master_station_dir
        output_path=data/$master_station_dir
        for istas in `seq 2 $num_stats`;do
          sta_in_file=`echo $istas |awk '{print $1+1}'`
          line=`sed "${sta_in_file}q;d" $stat_lst`
          stnw=`echo $line |awk -F"," '{print $1}'`
          stnm=`echo $line |awk -F"," '{print $2}'`
          stcm=`echo $line |awk -F"," '{print $3}'`
          slaver_station_name=`echo $stnw $stnm $stcm |awk '{print $1"."$2"."$3}'`
          station_pair=`echo $master_station_name $slaver_station_name |awk '{print $1"_"$2}'`
          
          gstack_for_sum=`ls data/day000*/gstack_$station_pair.rsf`
          lstack_for_sum=`ls data/day000*/lstack_$station_pair.rsf`
          gstack_out=$output_path/gstack_$station_pair.rsf
          lstack_out=$output_path/lstack_$station_pair.rsf
          scale=`echo $num_dates | awk '{print 1.0/$1}'`
          
          echo "sfadd >$gstack_out <`echo $gstack_for_sum ` scale=$scale" >tmp.bash
          echo "sfadd >$lstack_out <`echo $lstack_for_sum ` scale=$scale" >>tmp.bash
          sh tmp.bash
          
          gtfreq_outfile=${output_path}/gtfreq_$station_pair.rsf
          ltfreq_outfile=${output_path}/ltfreq_$station_pair.rsf

          sftimefreq < $gstack_out > $gtfreq_outfile nw=50 dw=0.01 w0=0. rect=10 niter=100 phase=n
          sftimefreq < $lstack_out > $ltfreq_outfile nw=50 dw=0.01 w0=0. rect=10 niter=100 phase=n
          
          gfreq_outfile=${output_path}/gfreq_$station_pair.rsf
          lfreq_outfile=${output_path}/lfreq_$station_pair.rsf
          sfspectra < $gstack_out > $gfreq_outfile
          sfspectra < $lstack_out > $lfreq_outfile

          
          simi_for_cat=`ls data/day000*/sstack_$station_pair.rsf`
          gccf_for_cat=`ls data/day000*/gstack_$station_pair.rsf`
          lccf_for_cat=`ls data/day000*/lstack_$station_pair.rsf`
          simi_out=$output_path/simi_$station_pair.rsf
          lccf_out=$output_path/lccf_$station_pair.rsf
          gccf_out=$output_path/gccf_$station_pair.rsf
          echo "sfcat  >$simi_out axis=2 < `echo $simi_for_cat ` " >tmp.bash
          echo "sfcat  >$lccf_out axis=2 < `echo $lccf_for_cat `" >>tmp.bash
          echo "sfcat  >$gccf_out axis=2 < `echo $gccf_for_cat `" >>tmp.bash
          sh tmp.bash
        done
      fi
done
