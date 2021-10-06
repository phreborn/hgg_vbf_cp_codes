#!/bin/bash

basedir=/publicfs/atlas/atlasnew/higgs/hgg/chenhr/vbfcp/syst/shape/

mufiles=$(ls ${basedir}/csv/Collect_photonallsys1_1_1/ | grep -v para_Nom)
for f in ${mufiles};do
  > ${basedir}/csv/${f}
done

sequence=($(seq 1 1 82))
intvl=0
for sysSet in photonallsys1 photonallsys2
do
for init in ${sequence[@]};do
  fin=$((${init} + ${intvl}))
  jobName=Collect_${sysSet}_${init}_${fin}; echo ${jobName}
  for f in ${mufiles};do
    cat ${basedir}/csv/${jobName}/${f} >> ${basedir}/csv/${f}
  done
done
done

cp ${basedir}/csv/Collect_photonallsys1_1_1/para_Nom.csv ${basedir}/csv/
