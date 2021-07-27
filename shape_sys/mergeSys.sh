#!/bin/bash

basedir=/publicfs/atlas/atlasnew/higgs/hgg/chenhr/vbfcp/syst/shape/

mufiles=$(ls ${basedir}/csv/Collect_1_1/ | grep -v para_Nom)
for f in ${mufiles};do
  > ${basedir}/csv/${f}
done

sequence=($(seq 1 1 82))
intvl=0
for init in ${sequence[@]};do
  fin=$((${init} + ${intvl}))
  jobName=Collect_${init}_${fin}; echo ${jobName}
  for f in ${mufiles};do
    cat ${basedir}/csv/${jobName}/${f} >> ${basedir}/csv/${f}
  done
done

cp ${basedir}/csv/Collect_1_1/para_Nom.csv ${basedir}/csv/
