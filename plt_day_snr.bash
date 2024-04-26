#!/bin/bash
gmt gmtset ANNOT_FONT_SIZE_PRIMARY 12p
gmt gmtset ANNOT_OFFSET_PRIMARY 0.1c
gmt gmtset LABEL_FONT_SIZE 14p
gmt gmtset LABEL_OFFSET 0.15c
gmt gmtset HEADER_FONT_SIZE 16p
gmt gmtset HEADER_OFFSET -0.5c
gmt gmtset TICK_LENGTH -0.2c
gmt gmtset BASEMAP_TYPE plain
export SAC_DISPLAY_COPYRIGHT=0

station_pair=6J.1007.DPZ_6J.1259.DPZ
stalo=-96.434402
stla=33.270199
evlo=-98.136803
evla=32.004002

ndays=30
:>stacked_ccf.lst
for iday in `seq $ndays`;do
  day=`echo $iday | awk '{printf"day%05d",$1}'`
  lccf=data/$day/lstack_$station_pair.rsf
  gccf=data/$day/gstack_$station_pair.rsf
  sh rsf2sac.bash $lccf -120 0.25 $stla $stlo $evla $evlo
  sh rsf2sac.bash $gccf -120 0.25 $stla $stlo $evla $evlo
  noise1=`~/software/sactools_c/sac_me 44 76 $lccf.sac |awk '{print $2}'`
  signal1=`~/software/sactools_c/sac_me 11 44 $lccf.sac |awk '{print $2}'`
  noise2=`~/software/sactools_c/sac_me -76 -44 $lccf.sac |awk '{print $2}'`
  signal2=`~/software/sactools_c/sac_me -44 -11 $lccf.sac |awk '{print $2}'`
  snr1=`echo $noise1 $noise2 $signal1 $signal2 |awk '{printf"%f", sqrt(($4+$3)/($1+$2))}'`
  noise1=`~/software/sactools_c/sac_me 44 77 $gccf.sac |awk '{print $2}'`
  signal1=`~/software/sactools_c/sac_me 11 44 $gccf.sac |awk '{print $2}'`
  noise2=`~/software/sactools_c/sac_me -77 -44 $gccf.sac |awk '{print $2}'`
  signal2=`~/software/sactools_c/sac_me -44 -11 $gccf.sac |awk '{print $2}'`
  snr2=`echo $noise1 $noise2 $signal1 $signal2 |awk '{printf"%f", sqrt(($4+$3)/($1+$2))}'`
  echo $iday $lccf.sac $snr1 $gccf.sac $snr2 >> stacked_ccf.lst
done
nstat=`cat stacked_ccf.lst |wc -l`
tmin=-120
tmax=120
tmid=`echo $tmin $tmax | awk '{print ($1+$2)/2}'`
bounds=-R$tmin/$tmax/-1/$nstat


gmt begin Fig/convergent_day jpg
  gmt subplot begin 2x2 -Fs6c/8c,4c -A"(a)"+o-1./-.5 -Bx+l'Time(s)' -By+l'Number of days stacked' 
    gmt subplot set
    gmt basemap $bounds -BwSen
    cat stacked_ccf.lst |awk '{print $4}'| gmt sac -En -M1.c  -W0.25p,black
    cat stacked_ccf.lst | awk '{print -130,NR-1,$1}' | awk 'NR % 2 == 0' | gmt text -N
    cat stacked_ccf.lst | awk '{printf"%d,%f,%3.1f\n",-100,NR-1.4,$5}' | gmt text -N -F+f8p,blue -Gwhite
    echo $tmid 32.5 "Raw stack" | gmt text -N
    echo -150 15 "Day index" | gmt text -N -F+a90
    gmt subplot set
    gmt basemap $bounds -BwSen
    cat stacked_ccf.lst |awk '{print $2}'| gmt sac -En -M1.c  -W0.25p,black
    echo $tmid 32.5 "Stack with local attributes" | gmt text -N
    cat stacked_ccf.lst | awk '{print -130,NR-1,$1}' | awk 'NR % 2 == 0' | gmt text -N
    cat stacked_ccf.lst | awk '{printf"%d,%f,%3.1f\n",-100,NR-1.4,$3}' | gmt text -N -F+f8p,blue -Gwhite
    

    
  gmt subplot end

gmt end
