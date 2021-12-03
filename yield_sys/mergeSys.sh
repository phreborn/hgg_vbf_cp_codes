#!/bin/bash

basedir=/publicfs/atlas/atlasnew/higgs/hgg/chenhr/vbfcp/syst/yield/

files=$(ls ${basedir}/csv/Collect_photonsys_1_3/ | grep yield_)
for f in ${files};do
  > ${basedir}/csv/${f}
done

sequence=($(seq 1 3 54))
intvl=2
for sysSet in photonsys jetsys
do
for init in ${sequence[@]};do
  fin=$((${init} + ${intvl}))
  jobName=Collect_${sysSet}_${init}_${fin}; echo ${jobName}
  for f in ${files};do
    cat ${basedir}/csv/${jobName}/${f} >> ${basedir}/csv/${f}
  done
done
done

cp ${basedir}/csv/Collect_photonsys_1_3/N_yield.csv ${basedir}/csv/
