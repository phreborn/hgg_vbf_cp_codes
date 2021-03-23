#!/bin/bash

tmp=tmp.sh
> ${tmp}

d_tilde=$(ls yield_sys/csv/ | grep b3 | grep -v SM | grep TT | cut -d '_' -f 4)
cats=$(cat ../nom_WS/cats.cfg | grep -v "#" | grep ":" | cut -d ":" -f 1)

for d in ${d_tilde};do
for cat in ${cats};do
#for d in p01;do
#for cat in LL_b1;do
  file=config/vbf_cp_${d}/channel/category_OO_${cat}.xml; echo ${file}
  pos=$(sed -n '/nbkg_:category/=' ${file})
  old=$(grep "nbkg_:category" ${file} | cut -d , -f 1 | cut -d "[" -f 2)
  new=$(grep "${cat}" shape_sys/csv/para_bkg.csv | cut -d , -f 4)
  echo "sed -i '${pos} s/${old}/${new}/' ${file}" >> ${tmp}
done
done

if [ "$1" = "cmd" ];then
  cat ${tmp}
elif [ "$1" = "exe" ];then
  source ${tmp}
fi
rm ${tmp}
