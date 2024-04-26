#!/bin/bash

# get the data lst from noisepy

ls Download/RAW_DATA/*.h5 > data/rawdata.lst

ls Download/Preproc/*.h5 > data/preproc.lst
cp Download/RAW_DATA/station.txt data/

