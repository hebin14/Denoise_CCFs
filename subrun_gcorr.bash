#!/bin/bash
data_lst=data/alldata_for_gccf.lst
num_data=`cat $data_lst |wc -l`

nlen=`echo $num_data $PMI_SIZE | awk '{printf"%d",$1/$2}'`
echo 'runing lcorr on rank=' $PMI_RANK 'for number of pair:' $nlen
for ie in `seq 0 $nlen`;do
  ievent=`echo $ie $PMI_SIZE $PMI_RANK | awk '{print $1*$2+$3+1}'`
  if [ "$ievent" -le "$num_data" ];then
    line=`sed "${ievent}q;d" $data_lst`
    ccf_infile=`echo $line | awk '{print $1}'`
    ccf_other=`echo $line | awk '{print $2}'`
    gccf_outfile=`echo $line | awk '{print $3}'`
    gcorr_len=`echo $line | awk '{print $4}'`
    gcorr_overlap=`echo $line | awk '{print $5}'`
    nccfs=`echo $line | awk '{print $6}'`
    taumax=`echo $line | awk '{print $7}'`
    if [ -f "$ccf_infile" ] && [ -f "$ccf_other" ]; then
      ./gcorr <$ccf_infile  other=$ccf_other > $gccf_outfile Tau_max=$taumax nccfs=$nccfs gcorr_len=$gcorr_len gcorr_overlap=$gcorr_overlap --out=stdout
    fi
  fi
done
