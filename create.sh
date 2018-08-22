#!/bin/bash

set -o nounset


# PU = 140
TYPE=MinBias
Prefix=/store/mc/TTI2023Upg14D/Neutrino_Pt2to20_gun/GEN-SIM-DIGI-RAW/PU140bx25_PH2_1K_FB_V3-v2/00000/


let "i=0"

for TheFile in `cat MinBiasFiles.txt`
do

InputFile="\""$Prefix$TheFile"\""


let "i=i+1"

if [ $i -le 200 ]    # PU140
then
    cname="produce_""$TYPE""_""$i""_cfg.py"

    sed -e "s@TheInputFile@$InputFile@g" \
	    config.tmpl  > $cname

    jname="sub_""$TYPE""_""$i"
    sed -e "s@CONFIG_FILE@$cname@g" \
	-e "s@INDX@$i@g" \
	-e "s@TYPE@$TYPE@g" \
         submit_job.tmpl > $jname

    chmod 755 $jname
    echo "submitting job : "$jname

    bsub -q 8nh $jname   # PU140
    #bsub -q1nd $jname   # PU70, more events per file..
fi

done

