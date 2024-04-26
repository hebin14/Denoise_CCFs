#!/bin/bash
#SBATCH --ntasks=72
#SBATCH --time=03:00:00
#SBATCH --job-name ccf
#SBATCH --output=out2.log
#SBATCH --partition Optane-long

nproc=72
export LD_LIBRARY_PATH=/home/bxh220006/scratch/local_attribute:$LD_LIBRARY_PATH
echo "=================================================="
echo "gather all the data for ccf"

data_lst=./data/preproc.lst
stat_lst=./data/station.txt
num_dates=`cat $data_lst |wc -l`
num_stats=`cat $stat_lst |wc -l`
num_stats=`echo $num_stats | awk '{print $1-1}'`

minperiod=2.0
referencevs=3.0
sed -n '2,$p' $stat_lst > stat.lst
:>public_ccf.lst

num_stats=`cat stat.lst |wc -l`
echo 'collecting ccf pairs with num of stations: '$num_stats
num_stats_minus_one=`echo $num_stats | awk '{print $1-1}'`
for ista1 in `seq 1 1`;do
    sline=`sed "${ista1}q;d" stat.lst | awk -F"," '{print $1"."$2"."$3,$4,$5} '`
    master_station_name=`echo $sline | awk '{print $1}'`
    slat=`echo $sline | awk '{print $2}'`
    slon=`echo $sline | awk '{print $3}'`
    ista1_plus_one=`echo $ista1 | awk '{print $1+1}'`
    echo $master_station_name
    for ista2 in `seq $ista1_plus_one $num_stats`;do
        rline=`sed "${ista2}q;d" stat.lst | awk -F"," '{print $1"."$2"."$3,$4,$5} '`
        rlat=`echo $rline | awk '{print $2}'`
        rlon=`echo $rline | awk '{print $3}'`
        slaver_station_name=`echo $rline | awk '{print $1}'`
        dist=`~/software/sactools_c/distbaz $slon $slat $rlon $rlat | awk '{print $1}'`
        refdist=`echo $minperiod $referencevs | awk '{print 2.0*$1*$2}'`
        ccf_output_name=`echo $master_station_name $slaver_station_name |awk '{print $1"_"$2}'`
        if (( $(echo "$refdist < $dist" | bc -l ) )) ;then
            echo $ccf_output_name $dist >>public_ccf.lst
        fi
    done
done
