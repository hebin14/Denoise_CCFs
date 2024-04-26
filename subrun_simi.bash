#!/bin/bash

data_lst=data/alldata_for_similarity.lst
num_data=`cat $data_lst |wc -l`
echo 'runing similarity on rank=' $PMI_RANK 'for number of stations:' $num_data
nlen=`echo $num_data $PMI_SIZE | awk '{printf"%d",$1/$2}'`
for ie in `seq 0 $nlen`;do
  ievent=`echo $ie $PMI_SIZE $PMI_RANK | awk '{print $1*$2+$3+1}'`
  if [ "$ievent" -le "$num_data" ];then
    line=`sed "${ievent}q;d" $data_lst`
    lccf_infile=`echo $line | awk '{print $1}'`
    gccf_infile=`echo $line | awk '{print $2}'`
    simi_outfile=`echo $line | awk '{print $3}'`
    if [ -f "$lccf_infile" ] && [ -f "$gccf_infile" ] ; then
      ./similarity <$lccf_infile other=$gccf_infile rect1=10 rect2=20 niter=50 verb=n >$simi_outfile  --out=stdout
    fi
  fi
done
