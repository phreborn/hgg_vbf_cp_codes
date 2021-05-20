#!/bin/bash

files=$(ls csv/Collect_1_3/ | grep yield_)
for f in ${files};do
  > csv/${f}
done

sequence=($(seq 1 3 54))
intvl=2
for init in ${sequence[@]};do
  fin=$((${init} + ${intvl}))
  jobName=Collect_${init}_${fin}; echo ${jobName}
  for f in ${files};do
    cat csv/${jobName}/${f} >> csv/${f}
  done
done

cp csv/Collect_1_3/N_yield.csv csv/
