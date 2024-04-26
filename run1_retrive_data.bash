#!/bin/bash
#SBATCH --ntasks=1
#SBATCH --time=01:00:00
#SBATCH --job-name asdfio
#SBATCH --output=out.log
#SBATCH --partition Optane

export LD_LIBRARY_PATH=/home/bxh220006/scratch/local_attribute:$LD_LIBRARY_PATH

echo "=================================================="
sh get_data_lst.bash
data_lst=./data/preproc.lst
stat_lst=./data/station.txt
num_dates=`cat $data_lst |wc -l`
num_stats=`cat $stat_lst |wc -l`
num_stats=`echo $num_stats | awk '{print $1-1}'`
echo 'num of dates: '$num_dates 'with num of stations: ' $num_stats
small_tag=dpz
for iday in `seq 1 $num_dates`;do
    echo ".................................................."
    noise_day_in_path=`sed "${iday}q;d" $data_lst`
    if [ -f "$noise_day_in_path" ];then
    echo 'extracting data for day: '$iday 'from ' $noise_day_in_path
    noise_day_out_path=`echo $iday | awk '{printf"./data/day%05d",$1}'`
    if [ -d "$noise_day_out_path" ]; then
        rm -rf $noise_day_out_path
    fi
    mkdir $noise_day_out_path
    :>${noise_day_out_path}/station.lst
    for ista in `seq 1 $num_stats`;do
        sta_in_file=`echo $ista |awk '{print $1+1}'`
        line=`sed "${sta_in_file}q;d" $stat_lst`
        stnw=`echo $line |awk -F"," '{print $1}'`
        stnm=`echo $line |awk -F"," '{print $2}'`
        stcm=`echo $line |awk -F"," '{print $3}'`
        stla=`echo $line |awk -F"," '{print $4}'`
        stlo=`echo $line |awk -F"," '{print $5}'`
        station_name=`echo $stnw $stnm $stcm |awk '{print $1"."$2"."$3}'`
        noise_stat_out_path=`echo $station_name $noise_day_out_path |awk '{print $2"/"$1}'`
        echo $station_name $stla $stlo  >>${noise_day_out_path}/station.lst
        
        ifexist=`./asdfcheck h5path=$noise_day_in_path station_name=$station_name small_tag=$small_tag`
        if [ "$ifexist" -eq "1" ]; then
            echo 'extracting data for station: '${station_name}
            ./asdfio h5path=$noise_day_in_path station_name=$station_name > $noise_stat_out_path.rsf small_tag=$small_tag --out=stdout
        fi
    done
    fi
done
