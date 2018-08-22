#!/bin/bash

set -o nounset


# PU = 140
# WORKDIR=/afs/cern.ch/user/y/yuanc/HCAL/PFG_HF_timing/CMSSW_10_2_0_pre2/src/HcalTupleAnalyzer/work/
WORKDIR=/afs/cern.ch/work/y/yuanc/private/HCAL/PFG/
#EXE=main_iso
#EXE=main_method0
#EXE=main_m0
EXE=main_check
#InputFiles=/afs/cern.ch/user/y/yuanc/HCAL/PFG_HF_timing/CMSSW_10_2_0_pre2/src/HcalTupleAnalyzer/config/input_lists/JetHT.txt
#InputFiles=./task.txt
#InputFiles=./task_method0.txt
#InputFiles=./task_Prompt.txt
InputFiles=./task_m0.txt
OutPrefix=./inputFiles/input
OutPostfix=.txt
OUTPUTPre=/afs/cern.ch/work/y/yuanc/private/HCAL/PFG/output/
let "i=0"

for TheFile in `cat ${InputFiles}`
do

let "i=i+1"
#InputFile="\""$Prefix$TheFile"\""


jname=job/job
outFile="${OutPrefix}""$i""${OutPostfix}"
echo ${TheFile} > ${outFile}

jname="${jname}"$i
OUTPUT="${OUTPUTPre}""output"$i".root"
sed -e "s@FILE@${EXE}@g" \
    -e "s@INPUT@${outFile}@g" \
    -e "s@OUTPUT@${OUTPUT}@g" \
    HBTiming > ${jname}

chmod 755 $jname

#bsub -q 8nh "${WORKDIR}${jname}"
bsub -q 1nh "${WORKDIR}${jname}"
echo "Submitting "${WORKDIR}${jname}
#if [ $i -le 200 ]    # PU140
#then
#    cname="produce_""$TYPE""_""$i""_cfg.py"
#
#    sed -e "s@TheInputFile@$InputFile@g" \
#	    config.tmpl  > $cname
#
#    jname="sub_""$TYPE""_""$i"
#    sed -e "s@CONFIG_FILE@$cname@g" \
#	-e "s@INDX@$i@g" \
#	-e "s@TYPE@$TYPE@g" \
#         submit_job.tmpl > $jname
#
#    chmod 755 $jname
#    echo "submitting job : "$jname
#
#    bsub -q 8nh $jname   # PU140
    #bsub -q1nd $jname   # PU70, more events per file..
#fi
if [ -e core* ]
then 
  rm core*
fi
done
echo "Done, total number of files is: "$i
