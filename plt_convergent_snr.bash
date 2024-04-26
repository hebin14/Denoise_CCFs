#!/bin/bash
gmt gmtset ANNOT_FONT_SIZE_PRIMARY 12p
gmt gmtset ANNOT_OFFSET_PRIMARY 0.1c
gmt gmtset LABEL_FONT_SIZE 14p
gmt gmtset LABEL_OFFSET 0.15c
gmt gmtset HEADER_FONT_SIZE 16p
gmt gmtset HEADER_OFFSET -0.5c
gmt gmtset TICK_LENGTH -0.2c
gmt gmtset BASEMAP_TYPE plain

datainfo=`gmt gmtinfo -C convergent_lstacking_snr.lst`
xmin=`echo $datainfo | awk '{print $1-1}'`
xmax=`echo $datainfo | awk '{print $2+1}'`
y1min=0.4
y1max=1.1
y2min=0
y2max=25

bounds1=-R$xmin/$xmax/$y1min/$y1max
bounds2=-R$xmin/$xmax/$y2min/$y2max
echo $bounds1 $bounds2

echo S 0.1c a 0.2c black 0.8p - raw stack >legend.dat
echo S 0.1c c 0.2c -  0.8p,red - stack with local attributes  >>legend.dat

gmt begin Fig/convergent_snr jpg
  gmt subplot begin 2x1 -Fs12c/6c -A"(a)" -Cs0.5c
    gmt subplot set
    gmt basemap $bounds1 -Bx5+l'Number of Days' -BSnWe -By0.2+l'Correlation Coefficien'
    cat convergent_gstacking_snr.lst | awk '{print $1, $3}' | gmt plot -Sa0.2c -Gblack 
    cat convergent_gstacking_snr.lst | awk '{print $1, $3}' | gmt plot -Wblack
    cat convergent_lstacking_snr.lst | awk '{print $1, $3}' | gmt plot -Sc0.2c -Wred
    cat convergent_lstacking_snr.lst | awk '{print $1, $3}' | gmt plot -Wred
    gmt legend legend.dat -DjBM+w6c+o0.0c/-2.0c -F+p0.3p --FONT=12p,4,black
    gmt subplot set
    gmt basemap $bounds2 -Bx5+l'Number of Days' -BSnWe -By+l'Signal to noise ratio'
    cat convergent_gstacking_snr.lst | awk '{print $1, $2}' | gmt plot -Sa0.2c -Gblack
    cat convergent_gstacking_snr.lst | awk '{print $1, $2}' | gmt plot -Wblack
    cat convergent_lstacking_snr.lst | awk '{print $1, $2}' | gmt plot -Sc0.2c -Wred
    cat convergent_lstacking_snr.lst | awk '{print $1, $2}' | gmt plot -Wred
  gmt subplot end
gmt end

