#!/bin/bash

basedir=/publicfs/atlas/atlasnew/higgs/hgg/chenhr/vbfcp/syst/yield/

files=$(ls ${basedir}/csv/Collect_1_3/ | grep jd_ | grep yield_)
for f in ${files};do
  > ${basedir}/csv/${f}
done

sequence=($(seq 1 3 54))
intvl=2
for init in ${sequence[@]};do
  fin=$((${init} + ${intvl}))
  jobName=Collect_${init}_${fin}; echo ${jobName}
  for f in ${files};do
    cat ${basedir}/csv/${jobName}/${f} >> ${basedir}/csv/${f}
  done
done

cp ${basedir}/csv/Collect_1_3/jd_N_yield.csv ${basedir}/csv/
