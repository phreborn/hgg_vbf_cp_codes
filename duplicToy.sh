#!/bin/bash

itoy=1
if [ $1 == "-i" ];then itoy=$2;fi

d_tilde=$(cat Dtilde | grep -v "#")
cats=$(cat ../nom_WS/cats.cfg | grep -v "#" | grep ":" | cut -d ":" -f 1)

basepath=/publicfs/atlas/atlasnew/higgs/hgg/chenhr/vbfcp/WSconfigs/configToys/
#exampath=${basepath}/configToy0/
exampath=config/
for i in ${itoy}
do
  dirname=configToy${i}
  if [ ! -d ${basepath}/configToy${i} ];then mkdir ${basepath}/configToy${i};fi
  echo "copying toy ${i} ..."
  cp ${exampath}/vbf_cp_* ${basepath}/configToy${i} -r
  echo "start substitution"
  for d in ${d_tilde}
  do
    sed -i "s/\/publicfs\/atlas\/atlasnew\/higgs\/hgg\/chenhr\/vbfcp\/WSconfigs\/configToys\/configToy${i}//g" ${basepath}/configToy${i}/vbf_cp_${d}/d_tilde_${d}.xml
    sed -i "s/config/\/publicfs\/atlas\/atlasnew\/higgs\/hgg\/chenhr\/vbfcp\/WSconfigs\/configToys\/configToy${i}/g" ${basepath}/configToy${i}/vbf_cp_${d}/d_tilde_${d}.xml

    sed -i "s/wsToy${i}//g" ${basepath}/configToy${i}/vbf_cp_${d}/d_tilde_${d}.xml
    sed -i "s/workspace/wsToy${i}/g" ${basepath}/configToy${i}/vbf_cp_${d}/d_tilde_${d}.xml

    for cat in ${cats}
    do
      echo ${d}_${cat}
      fcat=${basepath}/configToy${i}/vbf_cp_${d}/channel/category_OO_${cat}.xml
      if [[ ${cat} =~ "TT" || ${cat} =~ "TL" ]];then sed -i "s/toy_${cat}_0/toy_${cat}_${i}/" ${fcat};fi

      sed -i "s/\/publicfs\/atlas\/atlasnew\/higgs\/hgg\/chenhr\/vbfcp\/WSconfigs\/configToys\/configToy${i}//g" ${fcat}
      sed -i "s/config/\/publicfs\/atlas\/atlasnew\/higgs\/hgg\/chenhr\/vbfcp\/WSconfigs\/configToys\/configToy${i}/g" ${fcat}
    done
  done
done
