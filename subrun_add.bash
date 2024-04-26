#!/bin/bash

data_lst=data/subadd.lst
num_data=`cat $data_lst |wc -l`
echo 'runing add on rank=' $PMI_RANK 'for number of stations:' $num_data
nlen=`echo $num_data $PMI_SIZE | awk '{printf"%d",$1/$2}'`
for ie in `seq 0 $nlen`;do
  ievent=`echo $ie $PMI_SIZE $PMI_RANK | awk '{print $1*$2+$3+1}'`
  if [ "$ievent" -le "$num_data" ];then
    station_pair=`sed "${ievent}q;d" $data_lst`
    ls data/day000*/gccf_$station_pair.rsf > gccf_$station_pair.lst
    master_station_dir=`echo $station_pair | awk -F"_" '{print "data/"$1}'`
    gccf_out=$master_station_dir/refgccf_$station_pair.rsf
    ./myadd ccfile=gccf_$station_pair.lst >$gccf_out  --out=stdout
    echo $ievent/$num_data/$PMI_RANK 'finished...'
    rm gccf_$station_pair.lst

  fi
done


