#!/bin/bash
data_lst=data/alldata_for_sstack.lst
num_data=`cat $data_lst |wc -l`
echo 'runing gcorr on rank=' $PMI_RANK 'for number of station pairs:' $num_data
nlen=`echo $num_data $PMI_SIZE | awk '{printf"%d",$1/$2}'`

for ie in `seq 0 $nlen`;do
  ievent=`echo $ie $PMI_SIZE $PMI_RANK | awk '{print $1*$2+$3+1}'`
  if [ "$ievent" -le "$num_data" ];then
    line=`sed "${ievent}q;d" $data_lst`
    ccf_infile=`echo $line | awk '{print $1}'`
    ccf_other=`echo $line | awk '{print $2}'`
    stacked_infile=`echo $line | awk '{print $3}'`
    stack_outfile=`echo $line | awk '{print $4}'`
    dist=`echo $line | awk '{print $5}'`
    num_dates=`echo $line | awk '{print $6}'`
    flod=`echo $line | awk '{print $7}'`
    taumax=`echo $line | awk '{print $8}'`
    vsmin=`echo $line | awk '{print $9}'`
    vsmax=`echo $line | awk '{print $10}'`
    periodmax=`echo $line | awk '{print $11}'`
    gcorr_len=`echo $line | awk '{print $12}'`
    gcorr_overlap=`echo $line | awk '{print $13}'`
    ./stack_rms <$ccf_infile  other=$ccf_other stack=$stacked_infile > $stack_outfile dist=$dist num_dates=$num_dates Tau_max=$taumax vsmin=$vsmin vsmax=$vsmax periodmax=$periodmax fold_ccf=$fold gcorr_len=$gcorr_len gcorr_overlap=$gcorr_overlap --out=stdout
  fi
done
