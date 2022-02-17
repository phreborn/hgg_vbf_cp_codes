#!/bin/bash

inY=/publicfs/atlas/atlasnew/higgs/hgg/chenhr/vbfcp/syst/yield/csv/SignalWindow/N_yield.csv

> ./tmpsub

dval=m00
indir=configAllCats/vbf_cp_${dval}/channel/
for file in $(ls ${indir} | grep category_)
do
  bdtcat=$(echo ${file} | cut -d _ -f 3)
  oobin=$(echo ${file} | cut -d _ -f 4 | cut -d . -f 1)
  tarf=${indir}/${file}
  
  srVBFSM=$(cat ${inY} | grep VBF_m00_${bdtcat}_${oobin} | cut -d , -f 2)
  srVBFRW=$(cat ${inY} | grep VBF_${dval}_${bdtcat}_${oobin} | cut -d , -f 2)
  srggH=$(cat ${inY} | grep ggH_${bdtcat}_${oobin} | cut -d , -f 2)
  
  fuVBFSM=$(cat ${tarf} | grep yield_VBF_SM | cut -d [ -f 2 | cut -d ] -f 1)
  fuVBFRW=$(cat ${tarf} | grep yield_VBF_RW | cut -d [ -f 2 | cut -d ] -f 1)
  fuggH=$(cat ${tarf} | grep yield_ggH_SM | cut -d [ -f 2 | cut -d ] -f 1)

echo ""
echo ${fuggH} to ${srggH}   
echo ${fuVBFSM} to ${srVBFSM}   
echo ${fuVBFRW} to ${srVBFRW}   

  echo "sed -i 's/yield_ggH_SM\[${fuggH}/yield_ggH_SM\[${srggH}/g' ${tarf}" >> tmpsub
  echo "sed -i 's/yield_ggH\[${fuggH}/yield_ggH\[${srggH}/g' ${tarf}" >> tmpsub
  echo "sed -i 's/yield_VBF_SM\[${fuVBFSM}/yield_VBF_SM\[${srVBFSM}/g' ${tarf}" >> tmpsub
  echo "sed -i 's/yield_VBF_RW\[${fuVBFRW}/yield_VBF_RW\[${srVBFRW}/g' ${tarf}" >> tmpsub

done

#cat tmpsub
source tmpsub
rm tmpsub
