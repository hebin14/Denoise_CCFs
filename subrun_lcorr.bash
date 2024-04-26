#!/bin/bash
data_lst=data/alldata_for_lccf.lst
num_data=`cat $data_lst |wc -l`
echo 'runing lcorr on rank=' $PMI_RANK 'for number of stations:' $num_data
nlen=`echo $num_data $PMI_SIZE | awk '{printf"%d",$1/$2}'`
for ie in `seq 0 $nlen`;do
  ievent=`echo $ie $PMI_SIZE $PMI_RANK | awk '{print $1*$2+$3+1}'`
  if [ "$ievent" -le "$num_data" ];then
    line=`sed "${ievent}q;d" $data_lst`
    ccf_infile=`echo $line | awk '{print $1}'`
    ccf_other=`echo $line | awk '{print $2}'`
    lccf_outfile=`echo $line | awk '{print $3}'`
    nccfs=`echo $line | awk '{print $4}'`
    taumax=`echo $line | awk '{print $5}'`
    gw=`echo $line | awk '{print $6}'`
    if [ -f "$ccf_infile" ] && [ -f "$ccf_other" ]; then
      ./lcorr <$ccf_infile  other=$ccf_other > $lccf_outfile Tau_max=$taumax nccfs=$nccfs gaussian_window=$gw --out=stdout
    fi
  fi
done
