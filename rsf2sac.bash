export SAC_DISPLAY_COPYRIGHT=0
path=$1
t0=$2
dt=$3
stla=$4
stlo=$5
evla=$6
evlo=$7

sfrsf2txt <$path tfile=tmp.txt
sac << LASTLINE
            readtable tmp.txt
            ch b     $t0
            ch delta $dt
            ch stla $stla
            ch stlo $stlo
            ch evla $evla
            ch evlo $evlo
            write $path.sac
            q
LASTLINE
