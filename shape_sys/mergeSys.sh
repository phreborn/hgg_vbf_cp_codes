#!/bin/bash

mufiles=$(ls /publicfs/atlas/atlasnew/higgs/hgg/chenhr/vbfcp/syst/shape/csv/Collect_1_1/ | grep -v para_Nom)
for f in ${mufiles};do
  > csv/${f}
done

sequence=($(seq 1 1 82))
intvl=0
for init in ${sequence[@]};do
  fin=$((${init} + ${intvl}))
  jobName=Collect_${init}_${fin}; echo ${jobName}
  for f in ${mufiles};do
    cat /publicfs/atlas/atlasnew/higgs/hgg/chenhr/vbfcp/syst/shape/csv/${jobName}/${f} >> csv/${f}
  done
done

cp /publicfs/atlas/atlasnew/higgs/hgg/chenhr/vbfcp/syst/shape/csv/Collect_1_1/para_Nom.csv csv/
