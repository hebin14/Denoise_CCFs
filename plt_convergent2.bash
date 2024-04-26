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

nstat=`cat convergent_gstacking.lst |wc -l`
tmin=-120
tmax=120
tmid=`echo $tmin $tmax | awk '{print ($1+$2)/2}'`
bounds=-R$tmin/$tmax/-1/$nstat
bounds_band=-R$tmin/$tmax/-1/5
period=8
cat convergent_sstacking.lst | awk '{print $3-2*a,$3+2*a} ' a=$period > sstack.time
nstats=`cat convergent_gstacking.lst | wc -l`
for band in T001_T003 T002_T005 T004_T008 T005_T010 T008_T016;do
  hp=`echo $band |awk '{printf"%d",substr($1,7,3)}'`
  lp=`echo $band |awk '{printf"%d",substr($1,2,3)}'`
  hf=`echo $lp | awk '{printf"%f",1.0/$1-0.05}'`
  lf=`echo $hp | awk '{printf"%f",1.0/$1}'` 
  for istat in `seq $nstats`;do
    gstack=`sed "${istat}q;d" convergent_gstacking.lst | awk '{print $2}'`
    lstack=`sed "${istat}q;d" convergent_lstacking.lst | awk '{print $2}'`
    sstack=`sed "${istat}q;d" convergent_sstacking.lst | awk '{print $2}'`
    sac<<eof
      r $gstack $lstack $sstack
      taper;bp co $lf $hf n 4 p 2; w append .$band;q
eof
  done
done
num_dates_for_sum=(1 2 3)
for daynum in ${num_dates_for_sum[@]};do

stack=gstack
day15=`cat convergent_${stack}ing.lst | sed "${daynum}q;d" | awk '{print $2}'`
ls  ${day15}.T001_T003 ${day15}.T002_T005 ${day15}.T004_T008 ${day15}.T005_T010 ${day15}.T008_T016   >band_${stack}ing.lst
:>snr_dummy
for trace in `cat band_${stack}ing.lst`;do
    noise1=`~/software/sactools_c/sac_me 44 76 $trace |awk '{print $2}'`
    signal1=`~/software/sactools_c/sac_me 11 44 $trace |awk '{print $2}'`
    noise2=`~/software/sactools_c/sac_me -76 -44 $trace |awk '{print $2}'`
    signal2=`~/software/sactools_c/sac_me -44 -11 $trace |awk '{print $2}'`
    snr=`echo $noise1 $noise2 $signal1 $signal2 |awk '{printf"%f", sqrt(($4+$3)/($1+$2))}'`
    echo $snr >>snr_dummy
done
paste band_${stack}ing.lst snr_dummy >band_${stack}_snr_$daynum

stack=lstack
day15=`cat convergent_${stack}ing.lst | sed "${daynum}q;d" | awk '{print $2}'`
ls  ${day15}.T001_T003 ${day15}.T002_T005 ${day15}.T004_T008 ${day15}.T005_T010 ${day15}.T008_T016 >band_${stack}ing.lst
:>snr_dummy
for trace in `cat band_${stack}ing.lst`;do
    noise1=`~/software/sactools_c/sac_me 44 76 $trace |awk '{print $2}'`
    signal1=`~/software/sactools_c/sac_me 11 44 $trace |awk '{print $2}'`
    noise2=`~/software/sactools_c/sac_me -76 -44 $trace |awk '{print $2}'`
    signal2=`~/software/sactools_c/sac_me -44 -11 $trace |awk '{print $2}'`
    snr=`echo $noise1 $noise2 $signal1 $signal2 |awk '{printf"%f", sqrt(($4+$3)/($1+$2))}'`
    echo $snr >>snr_dummy
done
paste band_${stack}ing.lst snr_dummy >band_${stack}_snr_$daynum

stack=sstack
day15=`cat convergent_${stack}ing.lst | sed "${daynum}q;d" | awk '{print $2}'`
ls  ${day15}.T001_T003 ${day15}.T002_T005 ${day15}.T004_T008 ${day15}.T005_T010 ${day15}.T008_T016 >band_${stack}ing.lst
:>snr_dummy
for trace in `cat band_${stack}ing.lst`;do
    noise1=`~/software/sactools_c/sac_me 44 76 $trace |awk '{print $2}'`
    signal1=`~/software/sactools_c/sac_me 11 44 $trace |awk '{print $2}'`
    noise2=`~/software/sactools_c/sac_me -76 -44 $trace |awk '{print $2}'`
    signal2=`~/software/sactools_c/sac_me -44 -11 $trace |awk '{print $2}'`
    snr=`echo $noise1 $noise2 $signal1 $signal2 |awk '{printf"%f", sqrt(($4+$3)/($1+$2))}'`
    echo $snr >>snr_dummy
done
paste band_${stack}ing.lst snr_dummy >band_${stack}_snr_$daynum

done

gmt begin Fig/convergent jpg
  gmt subplot begin 4x2 -Fs8c/4c,4c,4c,4c -A"(a)"+o-1./-.5 -Bx+l'Time(s)' -By+l'Number of days stacked' 
    gmt subplot set
    gmt basemap $bounds -BwSen
    cat convergent_gstacking.lst |awk '{print $2}'| gmt sac -En -M1.5c  -W0.25p,black
    cat convergent_gstacking.lst | awk '{print -130,NR-1,$1}' | gmt text -N
    echo $tmid 3.5 "Raw stack" | gmt text -N
    echo -150 1 "Number of days stacked" | gmt text -N -F+f8p+a90
    gmt subplot set
    gmt basemap $bounds -BwSen
    cat convergent_lstacking.lst |awk '{print $2}'| gmt sac -En -M1.5c  -W0.25p,black
    echo $tmid 3.5 "Stack with local attributes" | gmt text -N

    
    gmt subplot set
    gmt basemap $bounds_band -BwSen
    cat band_gstack_snr_1 | awk '{print $1}' | gmt sac -En -M2.5c  -W0.25p,red
    cat band_gstack_snr_1 | awk '{printf"%f %f %3.1f\n", 75, NR-0.5, $2}' | gmt text -F+f9p -N -G255/255/255
    gmt text -N -F+f9p+a45<<eof
      -140 4 8-16s
      -140 3 5-10s 
      -140 2 4-8s
      -140 1 2-5s
      -140 0 1-3s
eof

    gmt subplot set
    gmt basemap $bounds_band -BwSen
    cat band_lstack_snr_1 | awk '{print $1}' | gmt sac -En -M2.5c  -W0.25p,red
    cat band_lstack_snr_1 | awk '{printf"%f %f %3.1f\n", 75, NR-0.5, $2}' | gmt text -F+f9p -N -G255/255/255
    gmt text -N -F+f9p+a45<<eof
      -140 4 8-16s
      -140 3 5-10s 
      -140 2 4-8s
      -140 1 2-5s
      -140 0 1-3s
eof

    gmt subplot set
    gmt basemap $bounds_band -BwSen
    cat band_gstack_snr_2 | awk '{print $1}' | gmt sac -En -M2.5c  -W0.25p,red
    cat band_gstack_snr_2 | awk '{printf"%f %f %3.1f\n", 75, NR-0.5, $2}' | gmt text -F+f9p -N -G255/255/255
    gmt text -N -F+f9p+a45<<eof
      -140 4 8-16s
      -140 3 5-10s 
      -140 2 4-8s
      -140 1 2-5s
      -140 0 1-3s
eof

    gmt subplot set
    gmt basemap $bounds_band -BwSen
    cat band_lstack_snr_2 | awk '{print $1}' | gmt sac -En -M2.5c  -W0.25p,red
    cat band_lstack_snr_2 | awk '{printf"%f %f %3.1f\n", 75, NR-0.5, $2}' | gmt text -F+f9p -N -G255/255/255
    gmt text -N -F+f9p+a45<<eof
      -140 4 8-16s
      -140 3 5-10s 
      -140 2 4-8s
      -140 1 2-5s
      -140 0 1-3s
eof

    gmt subplot set
    gmt basemap $bounds_band -BwSen
    cat band_gstack_snr_3 | awk '{print $1}' | gmt sac -En -M2.5c  -W0.25p,red
    cat band_gstack_snr_3 | awk '{printf"%f %f %3.1f\n", 75, NR-0.5, $2}' | gmt text -F+f9p -N -G255/255/255
    gmt text -N -F+f9p+a45<<eof
      -140 4 8-16s
      -140 3 5-10s 
      -140 2 4-8s
      -140 1 2-5s
      -140 0 1-3s
eof

    gmt subplot set
    gmt basemap $bounds_band -BwSen
    cat band_lstack_snr_3 | awk '{print $1}' | gmt sac -En -M2.5c  -W0.25p,red
    cat band_lstack_snr_3 | awk '{printf"%f %f %3.1f\n", 75, NR-0.5, $2}' | gmt text -F+f9p -N -G255/255/255
    gmt text -N -F+f9p+a45<<eof
      -140 4 8-16s
      -140 3 5-10s 
      -140 2 4-8s
      -140 1 2-5s
      -140 0 1-3s
eof
   


    
  gmt subplot end

gmt end
