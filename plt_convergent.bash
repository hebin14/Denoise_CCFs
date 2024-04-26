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
tmin=0
tmax=60
tmid=`echo $tmin $tmax | awk '{print ($1+$2)/2}'`
bounds=-R$tmin/$tmax/-1/$nstat
period=2
gmt begin Fig/convergent jpg
  gmt subplot begin 1x3 -Fs6c/8c -A"(a)" -Bx+l'Time(s)' -By+l'Number of days stacked' 
    gmt subplot set
    gmt basemap $bounds -BWSen
    cat convergent_gstacking.lst |awk '{print $2}'| gmt sac -En -M1.5c  -W0.25p,black
    cat convergent_gstacking.lst | awk '{print -15,NR-1,$1}' | gmt text -N
    echo $tmid 8.5 "Raw stack" | gmt text -N
    gmt subplot set
    gmt basemap $bounds -BwSen
    cat convergent_lstacking.lst |awk '{print $2}'| gmt sac -En -M1.5c  -W0.25p,black
    echo $tmid 8.5 "Stack with local attributes" | gmt text -N
    cat convergent_lstacking.lst | awk '{print -15,NR-1,$1}' | gmt text -N

    gmt subplot set
    gmt basemap $bounds -BwSen
    cat convergent_sstacking.lst |awk '{print $2}'| gmt sac -En -M1.5c  -W0.25p,black
    echo $tmid 8.5 "Selective stack with rms" | gmt text -N
    cat convergent_sstacking.lst | awk '{print -15,NR-1,$1}' | gmt text -N
    cat convergent_sstacking.lst |awk '{printf"%f %f %f %f\n",$3-2*a,NR-1,0,1.0}' a=$period| gmt plot -Sy0.4 -W0.5p,magenta
    cat convergent_sstacking.lst |awk '{printf"%f %f %f %f\n",$3+2*a,NR-1,0,1.0}' a=$period| gmt plot -Sy0.4 -W0.5p,magenta
    
  gmt subplot end

gmt end
