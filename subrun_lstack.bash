#!/bin/bash
data_lst=data/alldata_for_lstack.lst
num_data=`cat $data_lst |wc -l`
echo 'runing gcorr on rank=' $PMI_RANK 'for number of station pairs:' $num_data
nlen=`echo $num_data $PMI_SIZE | awk '{printf"%d",$1/$2}'`

for ie in `seq 0 $nlen`;do
  ievent=`echo $ie $PMI_SIZE $PMI_RANK | awk '{print $1*$2+$3+1}'`
  if [ "$ievent" -le "$num_data" ];then
    line=`sed "${ievent}q;d" $data_lst`
    lccf_infile=`echo $line | awk '{print $1}'`
    gccf_infile=`echo $line | awk '{print $2}'`
    simi_infile=`echo $line | awk '{print $3}'`
    thld_outfile=`echo $line | awk '{print $4}'`
    lstack_outfile=`echo $line | awk '{print $5}'`
    gstack_outfile=`echo $line | awk '{print $6}'`
    if [ -f "$lccf_infile" ] && [ -f "$gccf_infile" ] && [ -f "$simi_infile" ];then
      ./mythreshold < $simi_infile >$thld_outfile pclip=20  --out=stdout
      ./lstack <$gccf_infile do_simistack=0 fold=0 >$gstack_outfile --out=stdout
      ./lstack <$lccf_infile do_simistack=1  other=$thld_outfile >$lstack_outfile --out=stdout
    fi
  fi
done
